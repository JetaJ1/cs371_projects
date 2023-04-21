const fs = require('fs');

fs.readFile("./PA3support/Cryptanalysis/ciphertext.txt", (error, output) => crackCipher(error, output));

// Source that might be helpful - displays English letter frequencies
// https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html
const alphabet = [
	{ letter: "a", replacement: "c" },
	{ letter: "b", replacement: "i" },
	{ letter: "c", replacement: "o" },
	{ letter: "d", replacement: "v" },
	{ letter: "e", replacement: "y" },
	{ letter: "f", replacement: "b" },
	{ letter: "g", replacement: "l" },
	{ letter: "h", replacement: "k" },
	{ letter: "i", replacement: "f" },
	{ letter: "j", replacement: "t" },
	{ letter: "k", replacement: "q" },
	{ letter: "l", replacement: "m" },
	{ letter: "m", replacement: "a" },
	{ letter: "n", replacement: "d" },
	{ letter: "o", replacement: "z" },
	{ letter: "p", replacement: "h" },
	{ letter: "q", replacement: "p" },
	{ letter: "r", replacement: "s" },
	{ letter: "s", replacement: "j" },
	{ letter: "t", replacement: "n" },
	{ letter: "u", replacement: "u" },
	{ letter: "v", replacement: "r" },
	{ letter: "w", replacement: "g" },
	{ letter: "x", replacement: "e" },
	{ letter: "y", replacement: "w" },
	{ letter: "z", replacement: "x" }
];

function crackCipher(error, output) {
	const outputAsStr = output.toString().toLowerCase();
	let decipheredStr = outputAsStr;

	let numCharacters = 0;
	const occurances = [];
	const proportions = [];

	for (let i = 0; i < alphabet.length; i++) {
		const letter = alphabet[i];
		occurances.push(getOccurances(outputAsStr, letter.letter));
		numCharacters += occurances[i];
	}

	for (let i = 0; i < occurances.length; i++) {
		const occurance = occurances[i];
		proportions.push(occurance / numCharacters);
	}

	// console.log(numCharacters);
	// console.log(occurances);

	for (let i = 0; i < proportions.length; i++) {
		const letter = alphabet[i];
		const proportion = proportions[i];
		console.log(`The letter ${ letter.letter } appears ${ (proportion * 100).toFixed(2) }% of the time`);
	}

	console.log("");

	for (let i = 0; i < alphabet.length; i++) {
		const letter = alphabet[i];

		if (!letter.replacement) {
			letter.replacement = "-";
		}

		// We don't use Regex `.replace()` because letters that *are* replaced
		// which are later selected *for* replacement will be overwritten
		for (let j = 0; j < outputAsStr.length; j++) {
			const char = outputAsStr[j];

			if (char == letter.letter) {
				decipheredStr = replaceChar(decipheredStr, letter.replacement, j);
			}
		}
	}

	console.log(decipheredStr);
}

function getOccurances(outputAsStr, letter) {
	const match = new RegExp(`${ letter }`, "g");
	return (outputAsStr.match(match) || []).length;
}

function replaceChar(decipheredStr, replacement, index) {
	const p1 = decipheredStr.substr(0, index);
	const p2 = decipheredStr.substr(index + 1);
		
	return p1 + replacement + p2;
}