/**
 * Speed up the `mkdocs build` process for testing by moving all except the first 4 folders in ./docs/classes and ./docs/libraries/ to a temporary folder.
 *
 * This is useful for modifying and testing the templates.
 */
const fs = require('fs');
const path = require('path');

const quantityToKeep = 4;
const docsDirectoriesToSpeedUp = ['classes', 'libraries'];
const tempDir = 'temp';

function speedup() {
  for (const docsDir of docsDirectoriesToSpeedUp) {
    const srcDir = path.join('docs', docsDir);
    const outputDir = path.join(tempDir, docsDir);

    fs.mkdirSync(outputDir, { recursive: true });

    const files = fs.readdirSync(srcDir);

    for (let i = 0; i < files.length; i++) {
      const file = files[i];
      const srcPath = path.join(srcDir, file);
      let outputPath = path.join(outputDir, file);

      if (i < quantityToKeep) {
        continue;
      }

      fs.renameSync(srcPath, outputPath);
      console.log(`Moving ${srcPath} to ${outputPath}`);
    }
  }
}

function restore() {
  for (const docsDir of docsDirectoriesToSpeedUp) {
    const srcDir = path.join(tempDir, docsDir);
    const outputDir = path.join('docs', docsDir);

    const files = fs.readdirSync(srcDir);

    for (const file of files) {
      const srcPath = path.join(srcDir, file);
      let outputPath = path.join(outputDir, file);

      fs.renameSync(srcPath, outputPath);
      console.log(`Moving ${srcPath} back to ${outputPath}`);
    }
  }
}

if (process.argv[2] === 'restore') {
  restore();
} else {
  speedup();
}
