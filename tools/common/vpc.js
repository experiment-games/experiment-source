
const fs = require('fs');
const path = require('path');

const projectFilesClient = [
  './src/game/client/client_base.vpc',
  './src/game/client/client_experiment.vpc',
  './src/engine/engine_patches_client.vpc',
];
const projectFilesServer = [
  './src/game/server/server_base.vpc',
  './src/game/server/server_experiment.vpc',
  './src/engine/engine_patches_server.vpc',
];

let projectFileClientContents = '';
let projectFileServerContents = '';

for (const file of projectFilesClient) {
  projectFileClientContents += fs.readFileSync(file, 'utf8');
}

for (const file of projectFilesServer) {
  projectFileServerContents += fs.readFileSync(file, 'utf8');
}

function getRealmByFile(file, noFallback) {
  // remove src\ from the beginning if it's there
  if (file.startsWith('src\\') || file.startsWith('src/'))
    file = file.substring(4);

  // Convert file to a regex where all slashes are checked
  const fileRegex = RegExp(file.replace(/\\/g, '[\\/\\\\]', 'i'));

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

  // If the file contains any slashes, let's try to determine the realm based on the filename without the path
  if (!noFallback)
  {
    const basename = path.basename(file);
    let realm;

    if (file.includes('/') || file.includes('\\')) {
      realm = getRealmByFile(`"${basename}"`, true);

      if (realm !== 'unknown') {
        return realm;
      }
    }

    // try for /{filename}"
    realm = getRealmByFile(`/${basename}"`, true);

    if (realm !== 'unknown') {
      return realm;
    }

    // Last try for \{filename}"
    realm = getRealmByFile(`\\${basename}"`, true);

    if (realm !== 'unknown') {
      return realm;
    }
  }

  return 'unknown';
}
exports.getRealmByFile = getRealmByFile;
