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

// Automatic managing
goFetchSomeMilk.protect(1000 * 60 * 10) // If the we have not received milk within ten minutes, stop waiting

// Manually managing
halt.resetTrip(1000 * 60 * 10);
goFetchSomeMilk();
halt.clearTrip();
```

License
-------
CC

