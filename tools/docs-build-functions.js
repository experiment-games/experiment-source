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
const luaBindingArgumentPattern = /LUA_BINDING_ARGUMENT(?:_WITH_EXTRA|_ENUM|_ENUM_DEFINE)?\(\s*([^\s]+)\s*,\s*(\d+)\s*,\s*(?:[^,\n]+,\s*)?"([^"]+)"\s*\)/g;
const luaBindingArgumentNillablePattern = /LUA_BINDING_ARGUMENT_NILLABLE\(\s*(\w+)\s*,\s*(\d+)\s*,\s*"([^"]+)"\s*\)/g;
const luaBindingArgumentWithDefaultPattern = /LUA_BINDING_ARGUMENT(?:_ENUM)?_WITH_DEFAULT\(\s*(\w+)\s*,\s*(\d+)\s*,\s*([^,\n]+)\s*,\s*"([^"]+)"\s*\)/g;
const luaBindingEndPattern = /^LUA_BINDING_END\((.*)\)$/m;
const luaBindingReturnPattern = /"([^"]+)", "([^"]+)"/g;

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

  // Build the file contents
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
    // Group the arguments by position
    const groupedArgs = args.reduce((acc, arg) => {
      if (!acc[arg.position]) {
        acc[arg.position] = [];
      }

      acc[arg.position].push(arg);
      return acc;
    }, {});

    // Build the yaml for each group
    return Object.entries(groupedArgs).map(([position, args]) => {
      let result = `- position: ${position}`;

      if (args.length > 1) {
        result += `\n  types:`;
      }

      args.forEach(arg => {
        let defaults = '';

        if (arg.defaultValue) {
          defaults = `\n  default: ${wrapQuotes(arg.defaultValue.trim())}`;
        }

        if (arg.isNillable) {
          defaults += `\n  nillable: true`;
        }

        if (args.length > 1) {
          result += `\n    - name: ${wrapQuotes(arg.name.replace('.', '_').trim())}\n      type: ${arg.type}${defaults}`;
        } else {
          result += `\n  name: ${wrapQuotes(arg.name.replace('.', '_').trim())}\n  type: ${arg.type}${defaults}`;
        }
      });

      return result;
    }).join('\n');
  };

  if (func.arguments) {
    argumentSection = `arguments:\n${indentEachLine(buildArguments(func.arguments), 2)}`;
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
    .replace(/%description%/g, wrapQuotes(func.description))
    .replace(/%arguments%/g, argumentSection)
    .replace(/%returns%/g, returnSection)
    .replace(/%static%/g, staticSection)
    .replace(/%searchKeywords%/g, searchKeywords);

  const otherRealm = func.realm === 'client' ? 'server' : 'client';

  // If this function was already processed in this run, it's a shared function that has different realms
  // If the file contents differ, rename the files so their realms are included in the name
  if (functionsThisRun.has(filePath)) {
    if (fs.readFileSync(filePath, 'utf8') !== content.replace(/%realm%/g, otherRealm)) {
      const existingFunc = functionsThisRun.get(filePath);

      if (existingFunc.realm !== func.realm
        && existingFunc.concept === func.concept) {
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
    } else {
      // If the contents are the same, just change the realm to shared
      func.realm = 'shared';
    }
  }

  func.filePath = filePath;

  functionsThisRun.set(func.filePath, func);

  fs.mkdirSync(path.dirname(filePath), { recursive: true });
  fs.writeFileSync(filePath, content.replace(/%realm%/g, func.realm));
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
    case 'luaL_checkint':
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
    case 'luaL_opttrace':
    case 'lua_totrace':
      return 'Trace';
    case 'luaL_checkdamageinfo':
    case 'lua_todamageinfo':
      return 'DamageInfo';
    case 'luaL_checkrecipientfilter':
      return 'RecipientFilter';
    case 'luaL_checkphysicsobject':
      return 'PhysicsObject';
    case 'luaL_checkphysicssurfaceprops':
    case 'lua_tophysicssurfaceprops':
      return 'PhysicsSurfacePropertiesHandle';
    case 'lua_tofirebulletsinfo':
      return 'FireBulletsInfo';
    case 'luaL_checkmovehelper':
    case 'lua_tomovehelper':
      return 'MoveHelper';
    case 'luaL_checkusercmd':
    case 'lua_tousercmd':
      return 'UserCommand';
    case 'luaL_checknetchannel':
    case 'lua_tonetchannel':
      return 'NetChannelInfo';
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
    case 'luaL_checkconcommand':
    case 'luaL_optconcommand':
    case 'lua_toconcommand':
      return 'ConsoleCommand';
    case 'luaL_checkconvar':
    case 'luaL_optconvar':
    case 'lua_toconvar':
      return 'ConsoleVariable';
    case 'luaL_checksteamfriends':
    case 'lua_tosteamfriends':
      return 'SteamFriendsHandle';
    case 'luaL_checkprojectedtexture':
    case 'lua_toprojectedtexture':
      return 'ProjectedTexture';
    case 'luaL_checkischeme':
    case 'luaL_optischeme':
    case 'lua_toischeme':
      return 'Scheme';
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
    case 'lua_tobf_read':
      return 'MessageReader';
    case 'luaL_checkbf_write':
    case 'lua_tobf_write':
      return 'MessageWriter';
    case 'lua_checktracestruct':
      return 'Trace';
    case 'luaL_checkfont':
    case 'luaL_optfont':
    case 'lua_tofont':
      return 'FontHandle';
    case 'luaL_checkaudiochannel':
    case 'luaL_optaudiochannel':
    case 'lua_toaudiochannel':
      return 'AudioChannel';

    // Panels
    case 'luaL_checkpanel':
    case 'luaL_optpanel':
    case 'lua_topanel':
      return 'Panel';
    case 'luaL_checkbutton':
    case 'luaL_optbutton':
    case 'lua_tobutton':
      return 'Button';
    case 'luaL_checkcheckbutton':
    case 'luaL_optcheckbutton':
    case 'lua_tocheckbutton':
      return 'CheckButton';
    case 'luaL_checkeditablepanel':
    case 'luaL_opteditablepanel':
    case 'lua_toeditablepanel':
      return 'EditablePanel';
    case 'luaL_checkframe':
    case 'luaL_optframe':
    case 'lua_toframe':
      return 'Frame';
    case 'luaL_checkhtml':
    case 'luaL_opthtml':
    case 'lua_tohtml':
      return 'Html';
    case 'luaL_checklabel':
    case 'luaL_optlabel':
    case 'lua_tolabel':
      return 'Label';
    case 'luaL_checkmodelimagepanel':
    case 'luaL_optmodelimagepanel':
    case 'lua_tomodelimagepanel':
      return 'ModelImagePanel';
    case 'luaL_checktextentry':
    case 'luaL_opttextentry':
    case 'lua_totextentry':
      return 'TextEntry';

    // Enumerations:
    case 'Activity':
      return 'enumeration/ACTIVITY';
    case 'ActivationType_t':
    case 'Button::ActivationType_t':
      return 'enumeration/ACTIVATION_TYPE';
    case 'Alignment':
    case 'vgui::Alignment':
    case 'vgui::Label::Alignment':
      return 'enumeration/PANEL_ALIGNMENT';
    case 'AutoResize_e':
    case 'Panel::AutoResize_e':
      return 'enumeration/AUTO_RESIZE';
    case 'ButtonCode_t':
    case 'KeyCode':
    case 'MouseCode':
      return 'enumeration/BUTTON';
    case 'CursorCode':
      return 'enumeration/CURSOR';
    case 'DOCK_TYPE':
    case 'Dock':
    case 'Dock::Type':
      return 'enumeration/DOCK_TYPE';
    case 'ShakeCommand_t':
      return 'enumeration/SHAKE_COMMAND';
    case 'MoveCollide_t':
      return 'enumeration/MOVE_COLLIDE';
    case 'MoveType_t':
      return 'enumeration/MOVE_TYPE';
    case 'SolidType_t':
      return 'enumeration/SOLID';
    case 'SolidFlags_t':
      return 'enumeration/SOLID_FLAG';
    case 'SkyboxVisibility_t':
      return 'enumeration/SKYBOX_VISIBILITY';
    case 'ENTITY_EFFECT':
      return 'enumeration/ENTITY_EFFECT';
    case 'ENTITY_FLAG':
      return 'enumeration/ENTITY_FLAG';
    case 'GESTURE_SLOT':
      return 'enumeration/GESTURE_SLOT';
    case 'IFileSystem::KeyValuesPreloadType_t':
    case 'KeyValuesPreloadType_t':
      return 'enumeration/KEY_VALUES_PRELOAD_TYPE';
    case 'OBSERVER_MODE':
      return 'enumeration/OBSERVER_MODE';
    case 'RenderMode_t':
      return 'enumeration/RENDER_MODE';
    case 'RenderFx_t':
      return 'enumeration/RENDER_EFFECTS';
    case 'COLLISION_GROUP':
    case 'Collision_Group_t':
      return 'enumeration/COLLISION_GROUP';
    case 'PinCorner_e':
    case 'Panel::PinCorner_e':
      return 'enumeration/PIN_CORNER';
    case 'PLAYER_ANIM':
      return 'enumeration/PLAYER_ANIMATION';
    case 'SearchPathAdd_t':
      return 'enumeration/SEARCH_PATH_ADD_TO';
    case 'FileWarningLevel_t':
      return 'enumeration/FILE_WARNING_LEVEL';
    case 'VGuiPanel_t':
      return 'enumeration/VGUI_PANEL';
    case 'FontDrawType_t':
      return 'enumeration/FONT_DRAW_TYPE';
    case 'SurfaceFeature_e':
    case 'ISurface::SurfaceFeature_e':
      return 'enumeration/SURFACE_FEATURE';
    case 'CollideType_t':
      return 'enumeration/COLLIDE_TYPE';
    case 'ShadowType_t':
      return 'enumeration/SHADOW_TYPE';
    case 'soundlevel_t':
      return 'enumeration/SOUND_LEVEL';
    case 'Class_T':
      return 'enumeration/AI_CLASS';
    case 'BLOOD_COLOR':
      return 'enumeration/BLOOD_COLOR';
    case 'USE_TYPE':
      return 'enumeration/USE_TYPE';
    case 'MaterialCullMode_t':
      return 'enumeration/CULL_MODE';

    // Define enumerations:
    case 'LIFE':
      return 'enumeration/LIFE';
    case 'EDICT_FLAG':
      return 'enumeration/EDICT_FLAG';
    case 'ENGINE_FLAG':
      return 'enumeration/ENGINE_FLAG';
    case 'SURFACE':
      return 'enumeration/SURFACE';
    case 'NETWORK_VARIABLE':
      return 'enumeration/NETWORK_VARIABLE_TYPE';
    case 'CHAT_MESSAGE_MODE':
      return 'enumeration/CHAT_MESSAGE_MODE';
    case 'STENCIL_OPERATION':
      return 'enumeration/STENCIL_OPERATION';
    case 'STENCIL_COMPARISON_FUNCTION':
      return 'enumeration/STENCIL_COMPARISON_FUNCTION';
    case 'USABILITY_TYPE::TYPE':
      return 'enumeration/USABILITY_TYPE';

    default:
      return 'unknown';
  }
}

// Returns will be something like:
//  "table", "The attachment information"
// Where the first part is the type, and the second part is the description
// This can repeat multiple times
function parseReturns(returns) {
  const returnValues = [];

  let match;
  while ((match = luaBindingReturnPattern.exec(returns)) !== null) {
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
        lines: '',
      };
    } else if (currentFunction && luaBindingEndPattern.test(line)) {
      if (!currentFunction) {
        throw new Error(`Found LUA_BINDING_END without a LUA_BINDING_BEGIN in ${file} at line ${i}`);
      }
      const match = line.match(luaBindingEndPattern);

      currentFunction.returns = parseReturns(match[1]);

      // Some functions in shared files may specify the realm in the LUA_BINDING_BEGIN
      if (!currentFunction.realm) {
        currentFunction.realm = getRealmByFile(file);
      }

      // Use the regex to find the arguments in .lines. We use luaBindingArgumentPattern, luaBindingArgumentNillablePattern and luaBindingArgumentWithDefaultPattern
      // to find the arguments
      const arguments = [];

      const fromTypeCheckerWithError = (typeChecker) => {
        const type = fromTypeChecker(typeChecker);

        if (type === 'unknown') {
          console.warn(`Unknown type checker ${typeChecker} in function ${currentFunction.function} (${file}:${i})`);
        }

        return type;
      }

      let argumentMatch;
      while ((argumentMatch = luaBindingArgumentPattern.exec(currentFunction.lines)) !== null) {
        const type = fromTypeCheckerWithError(argumentMatch[1]);
        arguments.push({
          type,
          position: parseInt(argumentMatch[2]),
          name: argumentMatch[3],
        });
      }

      while ((argumentMatch = luaBindingArgumentNillablePattern.exec(currentFunction.lines)) !== null) {
        const type = fromTypeCheckerWithError(argumentMatch[1]);
        arguments.push({
          type,
          position: parseInt(argumentMatch[2]),
          name: argumentMatch[3],
          isNillable: true,
        });
      }

      while ((argumentMatch = luaBindingArgumentWithDefaultPattern.exec(currentFunction.lines)) !== null) {
        const type = fromTypeCheckerWithError(argumentMatch[1]);
        arguments.push({
          type,
          position: parseInt(argumentMatch[2]),
          name: argumentMatch[4],
          defaultValue: argumentMatch[3],
        });
      }

      currentFunction.arguments = arguments;

      writeFunctionToFile(currentFunction);

      // Reset the function
      currentFunction = null;
    } else if (currentFunction) {
      currentFunction.lines += line + '\n';
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
