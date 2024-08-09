/*
    Run this script to traverse the ./src directory, looking for lua bindings
    and generating the basics of the documentation as markdown files.

    In the root of the project, run:
    node tools/docs-build-basics.js
 */

const fs = require('fs');
const path = require('path');

const srcDir = './src';
const outputDir = './docs';

const luaBindingBeginPattern = /LUA_BINDING_BEGIN\(\s*(\w+)\s*,\s*(\w+)\s*,\s*"(\w+)"\s*,\s*"([^"]+)"\s*\)/;
const luaBindingArgumentPattern = /LUA_BINDING_ARGUMENT\(\s*(\w+)\s*,\s*(\d+)\s*,\s*"([^"]+)"\s*\)/;
const luaBindingOptionalArgumentPattern = /LUA_BINDING_ARGUMENT_OPT\(\s*(\w+)\s*,\s*(\d+)\s*,\s*([^,]+)\s*,\s*"([^"]+)"\s*\)/;
const luaBindingEndPattern = /LUA_BINDING_END\((.*)\)$/m;

const projectFileClient = './src/game/client/client_base.vpc';
const projectFileServer = './src/game/server/server_base.vpc';

const projectFileClientContents = fs.readFileSync(projectFileClient, 'utf8');
const projectFileServerContents = fs.readFileSync(projectFileServer, 'utf8');

function traverseDirectory(dir) {
  if (dir.includes('node_modules') || dir.includes('.git')) {
    return;
  }

  fs.readdir(dir, (err, files) => {
    if (err) {
      console.error(`Error reading directory ${dir}: ${err.message}`);
      return;
    }

    files.forEach(file => {
      const filePath = path.join(dir, file);

      fs.stat(filePath, (err, stats) => {
        if (err) {
          console.error(`Error reading file stats ${filePath}: ${err.message}`);
          return;
        }

        if (stats.isDirectory()) {
          traverseDirectory(filePath);
        } else if (stats.isFile() && file.endsWith('.cpp')) {
          processBindingsInFile(filePath);
        }
      });
    });
  });
}

const markdownTemplate = `---
template: %template%
title: %title%
icon: lua-%realm%
tags:
  - lua
  - %realm%
  - needs-verification
  - needs-example
lua:
  library: %library%
  function: %function%
  realm: %realm%
  description: %description%
  %arguments%
  %returns%
---
`;

function indentEachLine(text, indentLevel) {
  return text.split('\n').map(line => ' '.repeat(indentLevel) + line).join('\n');
}

function wrapQuotes(text) {
  text = text.replace(/"/g, '\\"');
  return `"${text}"`;
}

function writeFunctionToFile(func) {
  const directory = func.concept === 'class' ? 'classes' : 'libraries';
  const filePath = path.join(outputDir, directory, func.library, `${func.function}.md`);

  // If the file already exists, don't overwrite it
  if (fs.existsSync(filePath) && !process.argv.includes('--force')) {
    console.warn(`Skipping function ${func.function} as it already exists at ${filePath} (use --force to overwrite)`);
    return;
  }

  const template = func.concept === 'class' ? 'lua-class-function.html' : 'lua-library-function.html';
  let argumentSection = '';

  const buildArguments = (args) => {
    return args.map(arg => {
      let defaults = '';

      if (arg.defaultValue) {
        defaults = `\n  default: ${wrapQuotes(arg.defaultValue)}`;
      }

      return indentEachLine(`- name: ${wrapQuotes(arg.name)}\n  type: ${arg.type}${defaults}`, 2);
    }).join('\n');
  };

  if (func.arguments) {
    argumentSection = `arguments:\n${indentEachLine(buildArguments(func.arguments), 2)}`;
  } else if (func.argumentSets) {
    argumentSection = `argumentSets:\n${func.argumentSets.map(set => indentEachLine(`- arguments:\n${buildArguments(set.arguments)}`, 4)).join('\n')}`;
  }

  let returnSection = '';

  if (func.returns) {
    returnSection = `returns:\n${indentEachLine(func.returns.map(ret => `- type: ${ret.type}\n  description: ${wrapQuotes(ret.description)}`).join('\n'), 4)}`;
  }

  const content = markdownTemplate.replace(/%template%/g, template)
    .replace(/%title%/g, func.function)
    .replace(/%library%/g, func.library)
    .replace(/%function%/g, func.function)
    .replace(/%realm%/g, func.realm)
    .replace(/%description%/g, wrapQuotes(func.description))
    .replace(/%arguments%/g, argumentSection)
    .replace(/%returns%/g, returnSection)
    .replace(/%concept%/g, func.concept);

  fs.mkdir(path.dirname(filePath), { recursive: true }, (err) => {
    if (err) {
      console.error(`Error creating directory ${path.dirname(filePath)}: ${err.message}`);
    } else {
      fs.writeFile(filePath, content, (err) => {
        if (err) {
          console.error(`Error writing file ${filePath}: ${err.message}`);
        }
      });
    }
  });

  console.log(`Wrote function ${func.function} to ${filePath}`);
}

function fromTypeChecker(typeChecker) {
  switch (typeChecker) {
    case 'luaL_checknumber':
    case 'luaL_optnumber':
    case 'lua_tonumber':
      return 'number';
    case 'luaL_checkstring':
    case 'luaL_optstring':
    case 'lua_tostring':
      return 'string';
    case 'luaL_checkboolean':
    case 'luaL_optboolean':
    case 'lua_toboolean':
      return 'boolean';
    case 'luaL_checkentity':
    case 'luaL_optentity':
    case 'lua_toentity':
    case 'luaL_checkanimating':
    case 'luaL_optanimating':
    case 'lua_toanimating':
      return 'Entity';
    case 'luaL_checkplayer':
    case 'luaL_optplayer':
    case 'lua_toplayer':
    case 'luaL_checkexperimentplayer':
    case 'luaL_optexperimentplayer':
      return 'Player';
    default:
      return 'unknown';
  }
}

// Returns will be something like:
//  "table", "The attachment information"
// Where the first part is the type, and the second part is the description
// This can repeat multiple times
function parseReturns(returns) {
  const returnPattern = /"([^"]+)", "([^"]+)"/g;
  const returnValues = [];

  let match;
  while ((match = returnPattern.exec(returns)) !== null) {
    returnValues.push({
      type: match[1],
      description: match[2],
    });
  }

  return returnValues;
}

function processBindingsInFile(file) {
  const fileContent = fs.readFileSync(file, 'utf8');
  const lines = fileContent.split('\n');

  let currentFunction = null;
  let currentFunctionArgumentSets = [];
  let currentFunctionArgumentSet = null;

  const newFunctionArgumentSet = () => {
    currentFunctionArgumentSet = {
      arguments: [],
    };
    currentFunctionArgumentSets.push(currentFunctionArgumentSet);
  };

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    if (luaBindingBeginPattern.test(line)) {
      const match = line.match(luaBindingBeginPattern);
      currentFunction = {
        library: match[1],
        function: match[2],
        concept: match[3],
        description: match[4],
      };
    } else if (luaBindingArgumentPattern.test(line)) {
      const match = line.match(luaBindingArgumentPattern);
      const typeChecker = match[1];
      const position = parseInt(match[2]);
      const name = match[3];

      if (position == 1 && currentFunction.concept === 'class') {
        // Skip the first argument for classes, as it's always the class instance itself
        continue;
      }

      // Put the argument in the current argument set, or if that already
      // has a different type argument in the same position, create a new
      // argument set to indicate the function can be called with different
      // types in the same position.
      if (currentFunctionArgumentSet) {
        const existingArgument = currentFunctionArgumentSet.arguments.find(arg => arg.position === position);
        if (existingArgument && existingArgument.typeChecker !== typeChecker) {
          // Put this argument in a new set
          newFunctionArgumentSet();
        } else if (existingArgument) {
          // We've already pushed the argument set, so we don't need to do anything
          continue;
        }
      } else {
        newFunctionArgumentSet();
      }

      currentFunctionArgumentSet.arguments.push({
        type: fromTypeChecker(typeChecker),
        position,
        name,
      });
    } else if (luaBindingOptionalArgumentPattern.test(line)) {
      const match = line.match(luaBindingOptionalArgumentPattern);
      const typeChecker = match[1];
      const position = parseInt(match[2]);
      const defaultValue = match[3];
      const name = match[4];

      if (position == 1 && currentFunction.concept === 'class') {
        // Skip the first argument for classes, as it's always the class instance itself
        continue;
      }

      // Put the argument in the current argument set, or if that already
      // has a different type argument in the same position, create a new
      // argument set to indicate the function can be called with different
      // types in the same position.
      if (currentFunctionArgumentSet) {
        const existingArgument = currentFunctionArgumentSet.arguments.find(arg => arg.position === position);
        if (existingArgument && existingArgument.typeChecker !== typeChecker) {
          // Put this argument in a new set
          newFunctionArgumentSet();
        } else if (existingArgument) {
          // We've already pushed the argument set, so we don't need to do anything
          continue;
        }
      } else {
        newFunctionArgumentSet();
      }

      currentFunctionArgumentSet.arguments.push({
        type: fromTypeChecker(typeChecker),
        position,
        name,
        defaultValue,
      });
    } else if (luaBindingEndPattern.test(line)) {
      const match = line.match(luaBindingEndPattern);

      // If there's only one set, push it to 'arguments'
      if (currentFunctionArgumentSets.length === 1) {
        currentFunction.arguments = currentFunctionArgumentSets[0].arguments;
      }

      // If there are multiple sets, push them to 'argumentSets'
      if (currentFunctionArgumentSets.length > 1) {
        currentFunction.argumentSets = currentFunctionArgumentSets;
      }

      currentFunction.returns = parseReturns(match[1]);
      currentFunction.realm = getRealmByFile(file);

      writeFunctionToFile(currentFunction);

      // Reset the function
      currentFunction = null;
      currentFunctionArgumentSets = [];
      currentFunctionArgumentSet = null;
    }
  }
}

function getRealmByFile(file) {
  // remove src\ from the beginning if it's there
  if (file.startsWith('src\\') || file.startsWith('src/'))
    file = file.substring(4);

  // Convert file to a regex where all slashes are checked [/\\]
  const fileRegex = RegExp(file.replace(/\\/g, '[/\\\\]'));

  const inClient = fileRegex.test(projectFileClientContents);
  const inServer = fileRegex.test(projectFileServerContents);

  if (inClient && inServer) {
    console.log(`File ${file} is in both client and server`);
    return 'shared';
  }

  if (inClient) {
    return 'client';
  }

  if (inServer) {
    return 'server';
  }

  // If the file contains any slashes, let's try to determine the realm based on the filename without the path
  if (file.includes('/') || file.includes('\\')) {
    return getRealmByFile(`"${path.basename(file)}"`);
  }

  return 'unknown';
}

traverseDirectory(srcDir);
