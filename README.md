Halt
====
Halt is a node addon that makes it possible for the user to stop the execution of a V8 instance without kill the current loop.<br/>
Timeouts are managed by creating a parallel event loop who's sole objective is to manage the timer.<br/>
When a program is bottlenecked by the CPU, you may want to kill this event and that is the reason why we throw a nice CPU bound Exception on timeout.

Installation
------------
``` 
npm install halt
```

Compatibility
------
From node 0.12 - node 9 included<br/>
iojs 3

Timer
-------
When an engine is busy, a program needs more time to end but consumes the same amount of cpu ticks.<br/>
In this case we would like to give more time to this program to end his execution while he doesn't consume too many cpu ticks.<br/>
That's why the timer will be restarted until the maximum amount of cpu ticks is reached or the program ends his execution.<br/>
This maximum is determined by the value send to halt.start/run() and the cpu ticks per millisecond which is configurable and has 1000 as default value.

Usage
-----
You can either kill the javascript executing thread by 
* starting a timer on a function with halt.run()
* manually calling start, protect and clear on the halt module
```javascript
var halt = require('halt')
halt.ticksPerMsec = 1010;

function goFetchSomeMilk () { /* ... */; return milk; }

// In this case, max cpu ticks = 1000 * 60 * 10 * 1010;
// If max cpu ticks is reached, stop waiting

// Automatic managing
halt.run(goFetchSomeMilk, 1000 * 60 * 10)
 
// Manually managing
halt.start(1000 * 60 * 10);
halt.protect(goFetchSomeMilk);
halt.clear();
```

Exception
-------
When execution is forcefully terminated using TerminateExecution(), a catchable exception is generated.<br/>
In case you have sensible code and you want to recover the exception and catch it, you can use:
```javascript
var halt = require('halt')

function goFetchSomeMilk () { /* ... */; return milk; }

try{
    halt.start(50);
    try{
        halt.protect(goFetchSomeMilk);
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

