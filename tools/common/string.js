function indentEachLine(text, indentLevel) {
  return text.split('\n').map(line => ' '.repeat(indentLevel) + line).join('\n');
}
exports.indentEachLine = indentEachLine;

function wrapQuotes(text) {
  text = text.replace(/"/g, '\\"');
  return `"${text}"`;
}
exports.wrapQuotes = wrapQuotes;
