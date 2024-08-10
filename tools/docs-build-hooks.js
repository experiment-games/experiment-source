/*
    Run this script to traverse the ./src directory, looking for lua hooks
    and generating the basics of the documentation as markdown files.

    In the root of the project, run:
    node tools/docs-build-hooks.js
 */

const fs = require('fs');
const path = require('path');
const { getRealmByFile } = require('./common/vpc');
const { indentEachLine, wrapQuotes } = require('./common/string');
const { getAllFilesInDirectoriesAsMap, traverseDirectory } = require('./common/filesystem');

const srcDir = './src/';
const docsDir = './docs/';
const outputDir = path.join(docsDir, 'hooks');

const filesToUpdate = getAllFilesInDirectoriesAsMap(outputDir);
const hooksTouchedThisRun = new Map();

const luaHookBeginPattern = /LUA_CALL_HOOK_BEGIN\(\s*"([^"]+)"\s*(?:,\s*"([^"]+)")?\s*\)/;
// luaL_ or lua_, // doc: is optional
const luaPushPattern = /lua_?(\w+)\(\s*L,\s*([^)]+)\s*\);(?: \/\/ doc: (.+))?/;
// special case for pushing with CBaseEntity::PushLuaInstanceSafe(L, pEntity)
const luaPushInstancePattern = /::PushLuaInstanceSafe\(\s*L,\s*([^)]+)\s*\);(?:\s*\/\/\s*doc:\s*(.+))?/;
const luaHookEndPattern = /LUA_CALL_HOOK_END\(\s*(\d+)\s*,\s*(\d+)\s*\);(?:\s*\/\/\s*doc:\s*(.+))?/;

function fromPushFunction(functionName) {
  switch (functionName) {
    case 'pushinteger':
      return 'number';
    case 'pushnumber':
      return 'number';
    case 'pushstring':
      return 'string';
    case 'pushboolean':
      return 'boolean';
    case 'pushentity':
      return 'Entity';
    case 'pushplayer':
      return 'Player';
    case 'pushanimating':
      return 'Animating';
    case 'pushvector':
      return 'Vector';
    case 'pushangle':
      return 'Angle';
    case 'pushcolor':
      return 'Color';
    case 'pushnil':
      return 'nil';
    case 'pushmovedata':
      return 'MoveData';
    case 'pushusercmd':
      return 'UserCmd';
    case 'pushdamageinfo':
      return 'CTakeDamageInfo';
    case 'pushtrace':
      return 'CGameTrace';
    case 'pushsurfacedata':
      return 'surfacedata_t';
    default:
      return 'unknown';
  }
}

function fromPushLine(line) {
  const match = line.match(luaPushPattern);

  if (!match) {
    const instanceMatch = line.match(luaPushInstancePattern);

    if (instanceMatch) {
      return {
        type: 'entity',
        argument: instanceMatch[1].trim(),
        description: instanceMatch[2] || '',
      };
    }

    return null;
  }

  const type = fromPushFunction(match[1]);

  return {
    type: type,
    argument: match[2],
    description: match[3],
  };
}

function fromHookBeginLine(line) {
  const match = line.match(luaHookBeginPattern);

  if (!match) {
    return null;
  }

  return {
    name: match[1],
    description: wrapQuotes(match[2] || ''),
  };
}

function fromHookEndLine(line) {
  const match = line.match(luaHookEndPattern);

  if (!match) {
    return null;
  }

  if (!match[3]) {
    // Use match[2] to specify the number of return values (of unknown type)
    return Array.from({ length: parseInt(match[2]) }, () => ({ type: 'unknown', description: '' }));
  }

  return fromReturnsDescription(match[3]);
}

function fromReturnsDescription(description) {
  if (!description) {
    return [];
  }

  const returnValues = description.split(',').map(value => {
    const match = value.match(/(\w+)\s*\(([^)]+)\)/);

    if (match) {
      return {
        type: match[1],
        description: match[2],
      };
    }

    return {
      type: 'unknown',
      description: value.trim(),
    };
  });

  return returnValues;
}

const markdownTemplate = `---
template: lua-hook.html
title: %title%
icon: lua-%realm%
tags:
  - lua
  - %realm%
  - needs-verification
  - needs-example
lua:
  function: %title%
  realm: %realm%
  description: %description%
  %arguments%
  %returns%
---

<div class="lua__search__keywords">
hook %title% &#x2013; %description%
</div>
`;

const markdownTemplateRedirect = `---
template: lua-redirect.html
title: %title%
icon: lua-%realm%
tags:
  - lua
  - %realm%
  - needs-verification
lua:
  redirects:
    %redirects%
---

`;

function writeHookToFile(hook) {
  const filePath = path.join(outputDir, hook.realm, `${hook.name}.md`);

  // If the file already exists, don't overwrite it without --force
  if (fs.existsSync(filePath)
    && !process.argv.includes('--force')) {
    console.warn(`Skipping hook ${hook.name} as it already exists at ${filePath} (use --force to overwrite)`);
    return;
  }

  // Remove this file so we know it's up to date
  filesToUpdate.delete(filePath);

  // Track files touched this run, so we can find cases where a hook is in multiple files,
  // client and server, but not obviously shared. If we see a hook in multiple files, we
  // will add a file that simply redirects to the client- and server-specific hooks.
  if (hooksTouchedThisRun.has(hook.name)) {
    const otherHook = hooksTouchedThisRun.get(hook.name);
    let redirects = `- label: '${hook.realm}/${hook.name}'\n      url: '${hook.realm}/${hook.name}'`;
    redirects += `\n    - label: '${otherHook.realm}/${otherHook.name}'\n      url: '${otherHook.realm}/${otherHook.name}'`;

    const redirectFilePath = path.join(outputDir, 'shared', `${hook.name}.md`);
    const redirectContent = markdownTemplateRedirect.replace(/%title%/g, hook.name)
      .replace(/%realm%/g, 'shared')
      .replace(/%description%/g, hook.description)
      .replace(/%redirects%/g, redirects);

    fs.mkdirSync(path.dirname(redirectFilePath), { recursive: true });
    fs.writeFileSync(redirectFilePath, redirectContent);

    // Remove this file so we know it's up to date
    filesToUpdate.delete(redirectFilePath);
  }

  hooksTouchedThisRun.set(hook.name, hook);

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

  if (hook.arguments) {
    argumentSection = `arguments:\n${indentEachLine(buildArguments(hook.arguments), 2)}`;
  }

  let returnSection = '';

  if (hook.returns) {
    returnSection = `returns:\n${indentEachLine(hook.returns.map(ret => `- type: ${ret.type}\n  description: ${wrapQuotes(ret.description)}`).join('\n'), 4)}`;
  }

  const content = markdownTemplate.replace(/%title%/g, hook.name)
    .replace(/%realm%/g, hook.realm)
    .replace(/%description%/g, hook.description)
    .replace(/%arguments%/g, argumentSection)
    .replace(/%returns%/g, returnSection);

  fs.mkdirSync(path.dirname(filePath), { recursive: true });
  fs.writeFileSync(filePath, content);
}

function processHooksInFile(file) {
  const fileContent = fs.readFileSync(file, 'utf8');
  const lines = fileContent.split('\n');

  let currentHook = null;
  let currentArguments = [];
  let isBuildingTable = false;
  let countPushInBuildingTable = 0;

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    if (luaHookBeginPattern.test(line)) {
      currentHook = fromHookBeginLine(line);
    } else if (currentHook && line.includes('lua_newtable') && !isBuildingTable) {
      isBuildingTable = true;
      currentArguments.push({
        name: 'table',
        type: 'table',
      });
      continue;
    } else if (currentHook && (luaPushPattern.test(line) || luaPushInstancePattern.test(line))) {
      const push = fromPushLine(line, file, i);

      if (push.type === 'unknown') {
        // special case for tables
        if (line.includes('setfield') || line.includes('rawset') || line.includes('settable')) {
          // TODO: handle setting fields in tables
          countPushInBuildingTable--;
          continue;
        } else {
          console.warn(`Unknown pusher at ${file}:${i}: ${line.trim()}`);
          continue;
        }
      } else if (isBuildingTable) {
        countPushInBuildingTable++;
        if (countPushInBuildingTable === 2) {
          isBuildingTable = false;
          countPushInBuildingTable = 0;
        } else {
          continue;
        }
      }

      if (push) {
        currentArguments.push({
          name: push.argument,
          type: push.type,
        });
      }
    } else if (luaHookEndPattern.test(line)) {
      if (!currentHook) {
        throw new Error(`Found LUA_CALL_HOOK_END without a LUA_CALL_HOOK_BEGIN in ${file} at line ${i}`);
      }

      currentHook.arguments = currentArguments;
      currentHook.returns = fromHookEndLine(line);
      currentHook.realm = getRealmByFile(file);

      writeHookToFile(currentHook);

      // Reset the hook
      currentHook = null;
      currentArguments = [];
      isBuildingTable = false;
      countPushInBuildingTable = 0;
    }
  }
}

traverseDirectory(srcDir, processHooksInFile);

if (filesToUpdate.size > 0) {
  console.warn('The following files were not updated (maybe they\'re no longer relevant?):');

  for (const file of filesToUpdate.keys()) {
    console.warn(file);
  }
}
