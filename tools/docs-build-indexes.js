/*
  Run this script to traverse the ./docs directory and add index.md
  files to list child pages.

  In the root of the project, run:
  node tools/docs-build-indexes.js classes Methods
  node tools/docs-build-indexes.js libraries Functions
*/

const fs = require('fs');
const path = require('path');
const template = `---
template: lua-index.html
childrenTitle: %childrenTitle%
include:
  startsWith: %fullTargetSubDirectory%
exclude:
  isIndex: true
---

# %title%
`;

const outputDirectory = `./docs`;
const targetSubDirectory = process.argv[2] || `.`;
const childrenTitle = process.argv[3] || `Functions`;

function walkDirectory(directory) {
  const files = fs.readdirSync(directory);
  files.forEach(file => {
    const filePath = path.join(directory, file);
    const stats = fs.statSync(filePath);

    if (stats.isDirectory()) {
      walkDirectory(filePath);
    }
  });

  const indexFilePath = path.join(directory, `index.md`);

  if (fs.existsSync(indexFilePath)) {
    return;
  }

  const fullTargetSubDirectory = path.relative(outputDirectory, directory);
  let content = template
    .replace(`%childrenTitle%`, childrenTitle)
    .replace(`%title%`, path.basename(directory))
    .replace(`%fullTargetSubDirectory%`, fullTargetSubDirectory.replace(/\\/g, "/") + '/');

  fs.writeFileSync(indexFilePath, content);
  console.log(`Wrote ${indexFilePath}`);
}

walkDirectory(path.join(outputDirectory, targetSubDirectory));
