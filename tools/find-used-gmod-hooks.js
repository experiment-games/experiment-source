/*
    Run this script to scrap your Garry's Mod projects (specifically Helix schema's)
    to see which hooks are being used in your codebase.

    It'll be outputted to Hooks.txt in the same directory as this script.

    E.g:
    node find-used-gmod-Hooks.js "C:\Projects\gmod-experiment-redux" "C:\Projects\helix" "C:\Program Files (x86)\Steam\steamapps\common\GarrysMod\garrysmod\gamemodes\sandbox" "C:\Program Files (x86)\Steam\steamapps\common\GarrysMod\garrysmod\gamemodes\base"
*/

const fs = require('fs');
const path = require('path');

const targetPatterns = [
    /function GM:(\w+)\(([^\)]*)\)/g,
    /function Schema:(\w+)\(([^\)]*)\)/g,
    /function PLUGIN:(\w+)\(([^\)]*)\)/g,
    /hook\.Add\("(\w+)", "\w+", (.+)\)/g,
];

const outputFile = 'Hooks.txt';

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
                } else if (stats.isFile() && file.endsWith('.lua')) {
                    findPatternsInFile(filePath);
                }
            });
        });
    });
}

function findPatternsInFile(file) {
    fs.readFile(file, 'utf8', (err, data) => {
        if (err) {
            console.error(`Error reading file ${file}: ${err.message}`);
            return;
        }
        targetPatterns.forEach(pattern => {
            let match;
            while ((match = pattern.exec(data)) !== null) {
                logMatchToFile(file, match[0]);
            }
        });
    });
}

function logMatchToFile(file, match) {
    const logEntry = `${match}\n`;
    fs.appendFile(outputFile, logEntry, (err) => {
        if (err) {
            console.error(`Error writing to log file: ${err.message}`);
        }
    });
}

const startDirectories = process.argv.slice(2);

// Clear the output file before starting
fs.writeFile(outputFile, '', (err) => {
    if (err) {
        console.error(`Error initializing log file: ${err.message}`);
    } else {
        startDirectories.forEach(dir => {
            traverseDirectory(dir);
        });
    }
});
