var Module = require('./library.js');
//import Module from './library';


function test()
{
    let field = Module.createMyClass(10, 10);
    //let field = new Module.MineField(10, 10);
    try{
        console.log(field.toString());
        field.sweep(5, 5);
        console.log(field.toString());
    } catch (e) {
        console.log(e.stack);
    } finally {
        //field.delete();
    }
}

Module.onRuntimeInitialized = async _ => {
    test();
    await new Promise(resolve => setTimeout(resolve, 5000));
    test();
    console.log('done');
};