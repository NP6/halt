﻿var nativePath = process.env.HALT_NATIVE;
if (!nativePath) {
	if (process.platform === 'win32') {
		if (process.version.match(/^v0.12/)){
			nativePath = './native/win32/0.12/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v3/)){
			nativePath = './native/win32/3.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v4/)){
			nativePath = './native/win32/4.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v5/)){
			nativePath = './native/win32/5.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v6/)){
			nativePath = './native/win32/6.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v7/)){
			nativePath = './native/win32/7.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v8/)){
			nativePath = './native/win32/8.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v9/)){
			nativePath = './native/win32/9.x/' + process.arch + '/halt';
		}
	}
	else {
		nativePath = require('path').join(__dirname, '../build/Release/halt');
	}
}
module.exports = require(nativePath);