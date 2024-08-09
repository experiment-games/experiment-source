/*
    Run this script to traverse the ./src directory, looking for lua bindings
    and generating the basics of the documentation as markdown files.
 */

/*
Example code:

LUA_BINDING_BEGIN( CBaseAnimating, GetModelName, "class", "Get the model path of the entity" )
{
    lua_CBaseAnimating *pAnimating = LUA_BINDING_ARGUMENT( luaL_checkanimating, 1, "entity" );

    LUA_BINDING_MARK_RESULT( pAnimating->GetModelPtr()->pszName(), 1, "The model name" );

    return 1;
}
LUA_BINDING_END()

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
            LUA_BINDING_MARK_RESULT( lua_pushnil( L ), 1, "Attachment not found" );
            return 1;
        }
    }
    else if ( iArg2Type == LUA_TSTRING )
    {
        boneID = pAnimating->LookupAttachment( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "attachmentName" ) );

        if ( !pAnimating->GetAttachment( boneID, pVecOrigin, pVecAngles ) )
        {
            LUA_BINDING_MARK_RESULT( lua_pushnil( L ), 1, "Attachment not found" );
            return 1;
        }
    }
    else
    {
        luaL_typeerror( L, 2, "number or string" );
    }

    if ( boneID == -1 )
    {
        LUA_BINDING_MARK_RESULT( lua_pushnil( L ), 1, "Attachment not found" );
        return 1;
    }

    LUA_BINDING_MARK_RESULT( lua_newtable( L ), 1, "Attachment table" );
    lua_pushvector( L, pVecOrigin );
    lua_setfield( L, -2, "Pos" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushangle( L, pVecAngles );
    lua_setfield( L, -2, "Ang" );  // TODO: Write gmod compat and rename this to our own conventions
    lua_pushinteger( L, boneID );
    lua_setfield( L, -2, "Bone" );  // TODO: Write gmod compat and rename this to our own conventions

    return 1;
}
LUA_BINDING_END()
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
    - The number in LUA_BINDING_MARK_RESULT is the 1-based position of the return value. In case of duplicates we just pick the first description occurring.
 */

const fs = require('fs');
const path = require('path');

const srcDir = './src';
const outputDir = './docs';

const luaBindingBeginPattern = /LUA_BINDING_BEGIN\((\w+), (\w+), "(\w+)", "([^"]+)"\)/;
const luaBindingArgumentPattern = /LUA_BINDING_ARGUMENT\((\w+), (\d+), "([^"]+)"\)/g;
const luaBindingMarkResultPattern = /LUA_BINDING_MARK_RESULT\(([^,]+), (\d+), "([^"]+)"\)/;
const luaBindingEndPattern = /LUA_BINDING_END\(\)/;

const projectFileClient = 'src/game/client/client_base.vpc';
const projectFileServer = 'src/game/server/server_base.vpc';

const luaBindingFunctions = [];

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
                    findBindingsInFile(filePath);
                }
            });
        });
    });
}

function findBindingsInFile(file) {
    const fileContent = fs.readFileSync(file, 'utf8');
    const lines = fileContent.split('\n');

    let currentFunction = null;
    let currentFunctionArguments = [];
    let currentFunctionReturns = [];

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];

        if (luaBindingBeginPattern.test(line)) {
            const match = line.match(luaBindingBeginPattern);
            currentFunction = {
                class: match[1],
                function: match[2],
                realm: match[3],
                description: match[4],
            };
        } else if (luaBindingArgumentPattern.test(line)) {
            const match = line.match(luaBindingArgumentPattern);
            currentFunctionArguments.push({
                type: match[1],
                name: match[3],
            });
        } else if (luaBindingMarkResultPattern.test(line)) {
            const match = line.match(luaBindingMarkResultPattern);
            currentFunctionReturns[parseInt(match[2]) - 1] = {
                type: match[1],
                description: match[3],
            };
        } else if (luaBindingEndPattern.test(line)) {
            luaBindingFunctions.push({
                function: currentFunction,
                arguments: currentFunctionArguments,
                returns: currentFunctionReturns,
            });

            currentFunction = null;
            currentFunctionArguments = [];
            currentFunctionReturns = [];
        }
    }
}

function writeFunctionToFile(func) {
    const outputDir = path.join('classes', func.function.class);
    const outputFile = path.join(outputDir, `${func.function.function}.md`);

    const output = `---
template: lua-class-function.html
title: ${func.function.function}
lua:
    library: ${func.function.class}
    function: ${func.function.function}
    realm: ${func.function.realm}
    description: ${func.function.description}
    ${func.arguments.length > 0 ? 'argumentSets:' : ''}
    ${func.arguments.map((argument, i) => `
        - arguments:
            - name: ${argument.name}
              type: ${argument.type}
              description: ${argument.name}
    `).join('')}
    returns:
    ${func.returns.map((ret, i) => `
        - type: ${ret.type}
          description: ${ret.description}
    `).join('')}
---`;

    fs.mkdir(outputDir, { recursive: true }, (err) => {
        if (err) {
            console.error(`Error creating directory ${outputDir}: ${err.message}`);
            return;
        }

        fs.writeFile(outputFile, output, (err) => {
            if (err) {
                console.error(`Error writing to file ${outputFile}: ${err.message}`);
            }
        });
    });
}

// // Clear the output directory before starting
// fs.rmdir(outputDir, { recursive: true }, (err) => {
//     if (err) {
//         console.error(`Error clearing output directory ${outputDir}: ${err.message}`);
//     } else {
//         fs.mkdir(outputDir, { recursive: true }, (err) => {
//             if (err) {
//                 console.error(`Error creating output directory ${outputDir}: ${err.message}`);
//             } else {
//                 traverseDirectory(srcDir);
//             }
//         });
//     }
// });

traverseDirectory(srcDir);
