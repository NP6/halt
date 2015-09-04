Halt
====
Halt is a node addon that makes it possible for the user to stop the execution of a V8 instance.
Timeouts are managed by creating a parallel event loop who's sole objective is to manage the timer.
A nice CPU bound Exception is thrown on timeout


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

In case you have sensible code that needs to recover -- IL faudrait preciser ca -- 
you can use
```javascript
halt.protect(goFetchSomeMilk);
```

License
-------
![alt tag](https://licensebuttons.net/l/by/3.0/88x31.png)
This work is licensed under a [Creative Commons Attribution 4.0 International License] (http://creativecommons.org/licenses/by/4.0/)

