var nativePath = process.env.HALT_NATIVE;
if (!nativePath) {
	if (process.platform === 'win32') {
		if (process.version.match(/^v3/)){
			nativePath = './native/win32/3.x/' + process.arch + '/halt';
		}
		else if (process.version.match(/^v4/)){
			nativePath = './native/win32/4.x/' + process.arch + '/halt';
		}
	}
	else {
		nativePath = require('path').join(__dirname, '../build/Release/halt');
	}
}
module.exports = require(nativePath);