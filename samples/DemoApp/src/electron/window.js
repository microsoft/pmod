
var demoApp;
var addCommand;
var clearCommand;
var items;

function onItemsNotifyCollectionChanged(evt) {

    var itemsDiv = document.getElementById('itemsDiv');

    var action = evt.getAction();
    if (action == 1) {
        var item = evt.getNewItems()[0];
        var itemName = item.getProperty(1);

        var newdiv = document.createElement('div');
        newdiv.innerHTML = itemName;
        newdiv.className = "item";
        itemsDiv.appendChild(newdiv);
    }
    else if (action == 4) {

        while (itemsDiv.hasChildNodes()) {
            itemsDiv.removeChild(itemsDiv.lastChild);
        }
    }
}

function initDemoApp() {
    var pmodbin = '/tekchile/pmod.oss/bin/';
    var pmodNodeJs = pmodbin + 'Product/nodejs/x86/Debug/';

    var pmod = require(pmodNodeJs + 'pmod_nodejs_4e');
    pmod.loadLibrary(pmodNodeJs + 'pmod_lib');

    var demoAppBin = pmodbin + 'samples/DemoApp/classic/x86/Debug/';
    pmod.loadLibrary(demoAppBin + 'DemoApp');

    var demoAppFactory = pmod.getActivationFactory("samples.DemoApp");
    var dispatcher = new pmod.Dispatcher();
    demoApp = demoAppFactory.invokeMethod(1, dispatcher);

    // retrieve properties
    items = demoApp.getProperty(1);
    addCommand = demoApp.getProperty(2);
    clearCommand = demoApp.getProperty(5);

    // receive events from our items collection
    items.addNotifyCollectionChanged(onItemsNotifyCollectionChanged);
}

function doInitDemoApp(evt) {
    initDemoApp();
}

function addItem(evt) {
    var itemName = document.getElementById("itemName").value;
    addCommand.execute(itemName);
}

function clearItems(evt) {
    clearCommand.execute();
}

document.addEventListener('DOMContentLoaded', function () {
    initDemoApp();
    //document.getElementById("init").addEventListener("click", doInitDemoApp);
    document.getElementById("add").addEventListener("click", addItem);
    document.getElementById("clear").addEventListener("click", clearItems);
})
