/*
  See `/game/experiment/scripts/lua/utilities/portal_generate_enumerations.lua`
  for the script that generates markdown files to the game data folder.

  Due to how source engine writes, the path is forced lowercase.

  After running the Lua script, run this script to copy the files to the final
  destination, fixing the casing to be SCREAMING_SNAKE_CASE.

  node ./tools/docs-build-enumerations.js
*/

const fs = require('fs');
const path = require('path');

const srcDir = path.join('game', 'experiment', 'data', 'docs', 'enumerations');
const outputDir = path.join('docs', 'enumerations');

const files = fs.readdirSync(srcDir);

for (const file of files) {
  const srcPath = path.join(srcDir, file);
  const outputPath = path.join(outputDir, file.toUpperCase());

  fs.copyFileSync(srcPath, outputPath);
}
