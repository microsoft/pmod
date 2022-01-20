var pmodNodeJs = process.argv[2];
console.log('pmodNodeJs:' + pmodNodeJs);

var pmod = require(pmodNodeJs + 'pmod_nodejs');

pmod.loadLibrary('foo');

//Next, create a new instance of our object:
var dictionary = new pmod.Dictionary();

dictionary.insert('str', 'rodrigov');
dictionary.insert('int', 100);
dictionary.insert('double', 4.5);
dictionary.insert('yes', true);
dictionary.insert('false', false);

var oChild = new pmod.Dictionary();
oChild.insert('int', 200);
dictionary.insert('child', oChild);

console.log('int value: %d', dictionary.lookup('int'));
console.log('str value: %s', dictionary.lookup('str'));

oChild = dictionary.lookup('child');
console.log('int value: %d', oChild.lookup('int'));

