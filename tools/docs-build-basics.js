/*
    Run this script to traverse the ./src directory, looking for lua bindings
    and generating the basics of the documentation as markdown files.

    In the root of the project, run:
    node tools/docs-build-basics.js
 */

/*
Example code:

LUA_BINDING_BEGIN( CBaseAnimating, GetModelName, "class", "Get the model path of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    pAnimating->GetModelPtr()->pszName();

    return 1;
}
LUA_BINDING_END( "string", "The model name" )

LUA_BINDING_BEGIN( CBaseAnimating, GetAttachment, "class", "Get the attachment table for the specified attachment (by bone id or attachment name)" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    int iArg2Type = lua_type( L, 2 );
    Vector pVecOrigin;
    QAngle pVecAngles;
    int boneID = -1;

    if ( iArg2Type == LUA_TNUMBER )
    {
        boneID = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "boneId" );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
    }
    else if ( iArg2Type == LUA_TSTRING )
    {
        boneID = pAnimating->LookupAttachment( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentName" ) );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            lua_pushnil( L );
            return 1;
        }
    }
    else
    {
        luaL_typeerror( L, 2, "number or string" );
    }

    if ( boneID == -1 )
    {
        lua_pushnil( L );
        return 1;
    }

    lua_newtable( L );
    lua_pushvector( L, pVecOrigin );
    lua_setfield( L, -2, "Pos" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushangle( L, pVecAngles );
    lua_setfield( L, -2, "Ang" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushinteger( L, boneID );
    lua_setfield( L, -2, "Bone" );  // TODO: Write gmod compat and rename this to our own conventions

    return 1;
}
LUA_BINDING_END( "table", "The attachment information" )
*/

/*
 Example output (unrelated function):
    file: libraries/Bitwise/bor.md
    ---
    template: lua-library-function.html
    title: bor
    lua:
        library: Bitwise
        function: bor
        realm: shared
        description: Performs a bitwise OR operation on two numbers.
        arguments:
            - name: a
              type: number
              description: The first number.
            - name: b
              type: number
              description: The second number.
        returns:
            - type: number
              description: The result of the bitwise OR operation.
    ---

    So the output for the above example would be:
    file: classes/CBaseAnimating/GetModelName.md
    ---
    template: lua-class-function.html
    title: GetModelName
    lua:
        library: CBaseAnimating
        function: GetModelName
        realm: shared
        description: Get the model path of the entity.
        returns:
            - type: string
              description: The model name.
    ---

    file: classes/CBaseAnimating/GetAttachment.md
    ---
    template: lua-class-function.html
    title: GetAttachment
    lua:
        library: CBaseAnimating
        function: GetAttachment
        realm: shared
        description: Returns attachment information by the bone id or attachment name.
        argumentSets:
            - arguments:
                - name: boneID
                  type: number
                  description: The bone id.
            - arguments:
                - name: attachmentName
                  type: number
                  description: The attachment name.
        returns:
            - type: table
              description: The attachment information.
    ---

    Some things to note:
    - We determine the realm by checking if the file the function is in is found in `src/game/client/client_base.vpc` and/or `src/game/server/server_base.vpc`
    - With classes we skip the first argument, as it's always the class instance itself.
    - The number in LUA_BINDING_ARGUMENT marks the argument. In case of duplicates
 */

const fs = require('fs');
const path = require('path');

const srcDir = './src';
const outputDir = './docs';

const luaBindingBeginPattern = /LUA_BINDING_BEGIN\(\s*(\w+)\s*,\s*(\w+)\s*,\s*"(\w+)"\s*,\s*"([^"]+)"\s*\)/;
const luaBindingArgumentPattern = /LUA_BINDING_ARGUMENT\(\s*(\w+)\s*,\s*(\d+)\s*,\s*"([^"]+)"\s*\)/;
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

function writeFunctionToFile(func) {
  const directory = func.concept === 'class' ? 'classes' : 'libraries';
  const filePath = path.join(outputDir, directory, func.library, `${func.function}.md`);

  const template = func.concept === 'class' ? 'lua-class-function.html' : 'lua-library-function.html';
  let argumentSection = '';

  const buildArguments = (args) => {
    return args.map(arg => indentEachLine(`- name: ${arg.name}\n  type: ${arg.type}\n  description: ${arg.description}`, 2)).join('\n');
  };

  if (func.arguments) {
    argumentSection = `arguments:\n${indentEachLine(buildArguments(func.arguments), 2)}`;
  } else if (func.argumentSets) {
    argumentSection = `argumentSets:\n${func.argumentSets.map(set => indentEachLine(`- arguments:\n${buildArguments(set.arguments)}`, 4)).join('\n')}`;
  }

  let returnSection = '';

  if (func.returns) {
    returnSection = `returns:\n${indentEachLine(func.returns.map(ret => `- type: ${ret.type}\n  description: ${ret.description}`).join('\n'), 4)}`;
  }

  const content = markdownTemplate.replace(/%template%/g, template)
    .replace(/%title%/g, func.function)
    .replace(/%library%/g, func.library)
    .replace(/%function%/g, func.function)
    .replace(/%realm%/g, func.realm)
    .replace(/%description%/g, func.description)
    .replace(/%arguments%/g, argumentSection)
    .replace(/%returns%/g, returnSection);

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
      return 'number';
    case 'luaL_checkstring':
      return 'string';
    case 'luaL_checkboolean':
      return 'boolean';
    case 'luaL_checkentity':
    case 'luaL_checkanimating':
      return 'Entity';
    case 'luaL_checkplayer':
    case 'luaL_checkexperimentplayer':
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

      console.log("Found function", currentFunction);
      writeFunctionToFile(currentFunction);
    }
  }
}

function getRealmByFile(file) {
  // remove src\ from the beginning
  file = file.substring(4);

  // Convert file to a regex where all slashes are checked [/\\]
  const fileRegex = RegExp(file.replace(/\\/g, '[/\\\\]'));

  const inClient = fileRegex.test(projectFileClientContents);
  const inServer = fileRegex.test(projectFileServerContents);

  if (inClient && inServer) {
    return 'shared';
  }

  if (inClient) {
    return 'client';
  }

  if (inServer) {
    return 'server';
  }

  return 'unknown';
}

traverseDirectory(srcDir);
