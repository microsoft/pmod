function gcTest() {
    if (global.gc) {
        global.gc();
        console.log('global.gc');
    }
}

var pmodNodeJs = process.argv[2];
console.log('pmodNodeJs:' + pmodNodeJs);

var pmod = require(pmodNodeJs + 'pmod_nodejs');

pmod.loadLibrary(pmodNodeJs + 'pmod_lib');

var testModulePath = process.argv[3];
console.log('testModulePath:' + testModulePath);

pmod.loadLibrary(testModulePath);

var testModelFactory = pmod.getActivationFactory("testModel.Test");
var typeProperties = testModelFactory.invokeMethod(1);
console.log('Id:' + typeProperties.getUniqueId());

typeProperties.setProperty(6, 100);
var value = typeProperties.getProperty(6);
console.log('Int Property:' + value);

var propertyChangedFunc = function (evtArgs) {
    console.log('PropertyChanged Property:' + evtArgs.getProperty() + ' new value:' + evtArgs.getNewValue());
}

typeProperties.addPropertyChanged(propertyChangedFunc);
typeProperties.setProperty(6, 200);
typeProperties.removePropertyChanged(propertyChangedFunc);
typeProperties = null;

gcTest();

var dispatcher = new pmod.Dispatcher();

var typePropertiesTest = testModelFactory.invokeMethod(3, dispatcher);
typeProperties = typePropertiesTest.getProperty(1);
typeProperties.addPropertyChanged(propertyChangedFunc);
// invoke async setProperty
typePropertiesTest.invokeMethod(1, 6, 300);
setTimeout(function () {
    typeProperties.removePropertyChanged(propertyChangedFunc);
}, 100);

var methodSupport = testModelFactory.invokeMethod(2, dispatcher);

var asyncOperation = methodSupport.invokeMethod(10);
asyncOperation.onCompleted(function () {
    console.log('completed');
});


