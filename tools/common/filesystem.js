
const fs = require('fs');
const path = require('path');

// function getAllFilesInDirectoryAsMap(directory) {
//   let files = new Map();
//   let filesInDirectory = fs.readdirSync(directory, { withFileTypes: true });

//   for (let file of filesInDirectory) {
//     if (file.isDirectory()) {
//       files = new Map([...files, ...getAllFilesInDirectoryAsMap(path.join(directory, file.name))]);
//     } else {
//       files.set(path.join(directory, file.name), file);
//     }
//   }

//   return files;
// }
// exports.getAllFilesInDirectoryAsMap = getAllFilesInDirectoryAsMap;

function getAllFilesInDirectoriesAsMap(...arguments) {
  let files = new Map();

  for (let directory of arguments) {
    let filesInDirectory = fs.readdirSync(directory, { withFileTypes: true });

    for (let file of filesInDirectory) {
      if (file.isDirectory()) {
        files = new Map([...files, ...getAllFilesInDirectoriesAsMap(path.join(directory, file.name))]);
      } else {
        files.set(path.join(directory, file.name), file);
      }
    }
  }

  return files;
}
exports.getAllFilesInDirectoriesAsMap = getAllFilesInDirectoriesAsMap;

function traverseDirectory(dir, processor) {
  if (dir.includes('node_modules') || dir.includes('.git')) {
    return;
  }

  const files = fs.readdirSync(dir);

  for (const file of files) {
    const filePath = path.join(dir, file);
    const stats = fs.statSync(filePath);

    if (stats.isDirectory()) {
      traverseDirectory(filePath, processor);
    } else if (stats.isFile() && file.endsWith('.cpp')) {
      processor(filePath);
    }
  }
}
exports.traverseDirectory = traverseDirectory;
