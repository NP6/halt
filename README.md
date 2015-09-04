Halt
====
Halt is a node addon that makes it possible for the user to stop the execution of a V8 instance without kill the current loop.<br/>
Timeouts are managed by creating a parallel event loop who's sole objective is to manage the timer.<br/>
When a program is bottlenecked by the CPU, you may want to kill this event and that is the reason why we throw a nice CPU bound Exception on timeout


Installation
------------
``` 
npm install halt
```

Usage
-----
You can either kill the javascript executing thread by 
* starting a timer on a function
* manually calling start and stop on the halt module
```javascript
var halt = require('halt')

function goFetchSomeMilk () { /* ... */; return milk; }

// If the we have not received milk within ten minutes, stop waiting

// Automatic managing
goFetchSomeMilk.run(1000 * 60 * 10)
 
// Manually managing
halt.start(1000 * 60 * 10);
goFetchSomeMilk();
halt.clear();
```

Version 2 (coming soon)
-------
In case you have sensible code and you want to recover the exception and catch it, you can use
```javascript
halt.protect(goFetchSomeMilk);
```
example : 
```javascript
myMilk = goFetchSomeMilk(){
halt.start(50);
/*...*/;
halt.stop();
}

try{
halt.Start(50);
try{
halt.protect(myMilk);
}
catch(){
/* What ever you want */
}
halt.clear();
}


```

License
-------
![alt cc](https://licensebuttons.net/l/by/3.0/88x31.png)
This work is licensed under a [Creative Commons Attribution 4.0 International License] (http://creativecommons.org/licenses/by/4.0/)

