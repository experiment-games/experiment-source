/*
    Run this script to traverse the ./src directory, looking for lua bindings
    and generating the basics of the documentation as markdown files.

    In the root of the project, run:
    node tools/docs-build-functions.js
 */

const fs = require('fs');
const path = require('path');
const { getRealmByFile } = require('./common/vpc');
const { indentEachLine, wrapQuotes } = require('./common/string');
const { getAllFilesInDirectoriesAsMap, traverseDirectory } = require('./common/filesystem');

const srcDir = './src';
const outputDir = './docs';

const filesToUpdate = getAllFilesInDirectoriesAsMap(`${outputDir}/libraries`, `${outputDir}/classes`);
const functionsThisRun = new Map();

const luaBindingBeginPattern = /^LUA_BINDING_BEGIN\(\s*(\w+)\s*,\s*(\w+)\s*,\s*"([^"]+)"\s*,\s*"([^"]+)"\s*(?:,\s*"([^"]+)")?\s*\)/;
const luaBindingArgumentPattern = /LUA_BINDING_ARGUMENT(?:_WITH_EXTRA)?\(\s*(\w+)\s*,\s*(\d+)\s*,\s*(?:[^,]+,\s*)?"([^"]+)"\s*\)/;
const luaBindingArgumentNillablePattern = /LUA_BINDING_ARGUMENT_NILLABLE\(\s*(\w+)\s*,\s*(\d+)\s*,\s*"([^"]+)"\s*\)/;
const luaBindingArgumentWithDefaultPattern = /LUA_BINDING_ARGUMENT_WITH_DEFAULT\(\s*(\w+)\s*,\s*(\d+)\s*,\s*([^,]+)\s*,\s*"([^"]+)"\s*\)/;
const luaBindingEndPattern = /^LUA_BINDING_END\((.*)\)$/m;

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
  %static%
  %arguments%
  %returns%
---

<div class="lua__search__keywords">
%searchKeywords%
</div>
`;

const mergeClassInto = {
  CBaseAnimating: 'Entity',
  CBaseFlex: 'Entity',

  CExperimentPlayer: 'Player',
};

function getLibrary(func) {
  if (typeof mergeClassInto[func.library] !== 'undefined') {
    return mergeClassInto[func.library];
  }

  return func.library;
}

function writeFunctionToFile(func) {
  const directory = func.concept.startsWith('class') ? 'classes' : 'libraries';
  let filePath = path.join(outputDir, directory, getLibrary(func), `${func.function}.md`);

  // If the file already exists, don't overwrite it
  if (fs.existsSync(filePath) && !process.argv.includes('--force')) {
    console.warn(`Skipping function ${func.function} as it already exists at ${filePath} (use --force to overwrite)`);
    return;
  }

  // Remove this file so we know it's up to date
  filesToUpdate.delete(filePath);

  // If this function was already processed in this run, it's a shared function that has different realms
  // Rename the files so their realms are included in the name
  if (functionsThisRun.has(filePath)) {
    const existingFunc = functionsThisRun.get(filePath);

    if (existingFunc.realm !== func.realm) {
      const newFileName = `${existingFunc.function}.${existingFunc.realm}.md`;
      const newFilePath = path.join(outputDir, directory, getLibrary(func), newFileName);

      fs.renameSync(existingFunc.filePath, newFilePath);

      console.warn(`Renamed ${existingFunc.filePath} to ${newFilePath} to include realm ${existingFunc.realm}`);

      existingFunc.filePath = newFilePath;
      filesToUpdate.delete(existingFunc.filePath);

      // have our file path also include the realm
      const newFuncFileName = `${func.function}.${func.realm}.md`;
      filePath = path.join(outputDir, directory, getLibrary(func), newFuncFileName);
      filesToUpdate.delete(filePath);
    }
  }

  func.filePath = filePath;

  functionsThisRun.set(func.filePath, func);

  let templateFile;
  let staticSection = '';

  if (func.concept === 'class' || func.concept === 'class|static') {
    templateFile = 'lua-class-function.html';

    if (func.concept === 'class|static') {
      staticSection = '\n  static: true';
    }
  }
  else if (func.concept === 'library') {
    templateFile = 'lua-library-function.html';
  }
  else {
    throw new Error(`Unknown concept ${func.concept}`);
  }

  let argumentSection = '';

  const buildArguments = (args) => {
    return args.map(arg => {
      let defaults = '';

      if (arg.defaultValue) {
        defaults = `\n  default: ${wrapQuotes(arg.defaultValue.trim())}`;
      }

      if (arg.isNillable) {
        defaults += '\n  nillable: true';
      }

      return indentEachLine(`- name: ${wrapQuotes(arg.name.replace('.', '_').trim())}\n  type: ${arg.type}${defaults}`, 2);
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

  let searchKeywords = getLibrary(func);

  if (func.concept === 'class|static') {
    searchKeywords = `_R.${searchKeywords}`;
  }

  if (func.concept === 'class') {
    searchKeywords += `:${func.function}`;
  } else {
    searchKeywords += `.${func.function}`;
  }

  if (func.description)
    searchKeywords += ` &#x2013; ${func.description}`;

  const content = markdownTemplate.replace(/%template%/g, templateFile)
    .replace(/%title%/g, func.function)
    .replace(/%library%/g, getLibrary(func))
    .replace(/%function%/g, func.function)
    .replace(/%realm%/g, func.realm)
    .replace(/%description%/g, wrapQuotes(func.description))
    .replace(/%arguments%/g, argumentSection)
    .replace(/%returns%/g, returnSection)
    .replace(/%static%/g, staticSection)
    .replace(/%searchKeywords%/g, searchKeywords);

  fs.mkdirSync(path.dirname(filePath), { recursive: true });
  fs.writeFileSync(filePath, content);
}

function fromTypeChecker(typeChecker) {
  switch (typeChecker) {
    case 'luaL_checknumber':
    case 'luaL_optnumber':
    case 'lua_tonumber':
      return 'number';
    case 'luaL_checkinteger':
    case 'luaL_optinteger':
    case 'lua_tointeger':
      return 'integer';
    case 'lua_istable':
      return 'table';
    case 'luaL_checkstring':
    case 'luaL_optstring':
    case 'lua_tostring':
    case 'luaL_checklstring':
      return 'string';
    case 'luaL_checkboolean':
    case 'luaL_optboolean':
    case 'lua_toboolean':
      return 'boolean';
    case 'luaL_checkentity':
    case 'luaL_optentity':
    case 'lua_toentity':
      return 'Entity'; // (CBaseEntity)'; // Disabled because I'm not sure we want to be so specific in Lua
    case 'luaL_checkbaseflex':
    case 'luaL_optbaseflex':
    case 'lua_tobaseflex':
      return 'Entity'; // (CBaseFlex)';
    case 'luaL_checkanimating':
    case 'luaL_optanimating':
    case 'lua_toanimating':
      return 'Entity'; // (CBaseAnimating)';
    case 'lua_toweapon':
    case 'luaL_checkweapon':
      return 'Weapon'; // Entity (Weapon)';
    case 'luaL_checkplayer':
    case 'luaL_optplayer':
    case 'lua_toplayer':
      return 'Player'; // Entity (Player)';
    case 'luaL_checkexperimentplayer':
    case 'luaL_optexperimentplayer':
    case 'luaL_toexperimentplayer':
      return 'Player'; // Entity (CExperimentPlayer)';
    case 'luaL_checkeffect':
    case 'luaL_opteffect':
    case 'lua_toeffect':
      return 'EffectData';
    case 'luaL_checkvector':
    case 'luaL_optvector':
    case 'lua_tovector':
      return 'Vector';
    case 'luaL_checkangle':
    case 'luaL_optangle':
    case 'lua_toangle':
      return 'Angle';
    case 'luaL_checktrace':
      return 'Trace';
    case 'luaL_checkdamageinfo':
      return 'DamageInfo';
    case 'luaL_checkrecipientfilter':
      return 'RecipientFilter';
    case 'luaL_checkphysicsobject':
      return 'PhysicsObject';
    case 'lua_tofirebulletsinfo':
      return 'FireBulletsInfo';
    case 'luaL_checkcolor':
    case 'luaL_optcolor':
    case 'lua_tocolor':
      return 'Color';
    case 'luaL_checkfilehandle':
    case 'luaL_optfilehandle':
    case 'lua_tofilehandle':
      return 'FileHandle';
    case 'luaL_checkstringtable':
    case 'lua_tostringtable':
      return 'NetworkStringTable';
    case 'luaL_checkkeyvalues':
    case 'luaL_optkeyvalues':
    case 'lua_tokeyvalues':
      return 'KeyValuesHandle';
    case 'luaL_checkvmatrix':
    case 'luaL_optvmatrix':
    case 'lua_tovmatrix':
      return 'Matrix';
    case 'luaL_checkmatrix':
    case 'luaL_optmatrix':
    case 'lua_tomatrix':
      return 'Matrix3x4';
    case 'luaL_checkmovedata':
    case 'luaL_optmovedata':
    case 'lua_tomovedata':
      return 'MoveData';
    case 'lua_tosurfacedata':
      return 'SurfaceData';
    case 'luaL_checkitexture':
    case 'luaL_optitexture':
    case 'lua_toitexture':
      return 'Texture';
    case 'luaL_checkmaterial':
    case 'luaL_optmaterial':
    case 'lua_tomaterial':
      return 'Material';
    case 'lua_toperformanceparams':
      return 'PerformanceParameters';
    case 'luaL_checkphysicsobject':
    case 'luaL_optphysicsobject':
    case 'lua_tophysicsobject':
      return 'PhysicsObject';
    case 'luaL_checkrecipientfilter':
    case 'lua_torecipientfilter':
      return 'RecipientFilter';
    case 'luaL_checkbf_read':
    case 'luaL_optbf_read':
    case 'lua_tobf_read':
      return 'UserMessageReader';
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
        realm: match[5],
      };
    } else if (currentFunction && (luaBindingArgumentPattern.test(line) || luaBindingArgumentNillablePattern.test(line))) {
      const isNillable = luaBindingArgumentNillablePattern.test(line);
      const match = isNillable ? line.match(luaBindingArgumentNillablePattern) : line.match(luaBindingArgumentPattern);
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

      const type = fromTypeChecker(typeChecker);

      if (type === 'unknown')
        console.warn(`Unknown type checker ${typeChecker} in ${file}:${i}`);

      currentFunctionArgumentSet.arguments.push({
        type: type,
        position,
        name,
        isNillable,
      });
    } else if (currentFunction && luaBindingArgumentWithDefaultPattern.test(line)) {
      const match = line.match(luaBindingArgumentWithDefaultPattern);
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
    } else if (currentFunction && luaBindingEndPattern.test(line)) {
      if (!currentFunction) {
        throw new Error(`Found LUA_BINDING_END without a LUA_BINDING_BEGIN in ${file} at line ${i}`);
      }
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

      // Some functions in shared files may specify the realm in the LUA_BINDING_BEGIN
      if (!currentFunction.realm) {
        currentFunction.realm = getRealmByFile(file);
      }

      writeFunctionToFile(currentFunction);

      // Reset the function
      currentFunction = null;
      currentFunctionArgumentSets = [];
      currentFunctionArgumentSet = null;
    }
  }
}

traverseDirectory(srcDir, processBindingsInFile);

if (filesToUpdate.size > 0) {
  console.warn('The following files were not updated (maybe they\'re no longer relevant?):');

  for (const file of filesToUpdate.keys()) {
    if (!process.argv.includes('--list-unchanged-indexes') && file.includes('index.md'))
      continue;

    console.warn(file);
  }
}
