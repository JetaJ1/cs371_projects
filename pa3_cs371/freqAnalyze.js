const fs = require('fs');

fs.readFile("./Cryptanalysis/ciphertext.txt", (error, output) => doNext(error, output));

const alphabet = [
	{ letter: "a", replacement: "" },
	{ letter: "b", replacement: "" },
	{ letter: "c", replacement: "" },
	{ letter: "d", replacement: "" },
	{ letter: "e", replacement: "" },
	{ letter: "f", replacement: "" },
	{ letter: "g", replacement: "" },
	{ letter: "h", replacement: "" },
	{ letter: "i", replacement: "" },
	{ letter: "j", replacement: "t" },
	{ letter: "k", replacement: "" },
	{ letter: "l", replacement: "" },
	{ letter: "m", replacement: "u" },
	{ letter: "n", replacement: "" },
	{ letter: "o", replacement: "" },
	{ letter: "p", replacement: "h" },
	{ letter: "q", replacement: "" },
	{ letter: "r", replacement: "" },
	{ letter: "s", replacement: "" },
	{ letter: "t", replacement: "" },
	{ letter: "u", replacement: "f" },
	{ letter: "v", replacement: "" },
	{ letter: "w", replacement: "" },
	{ letter: "x", replacement: "e" },
	{ letter: "y", replacement: "" },
	{ letter: "z", replacement: "" }
];

function doNext(error, output) {
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

	console.log(numCharacters);
	console.log(occurances);

	for (let i = 0; i < proportions.length; i++) {
		const letter = alphabet[i];
		const proportion = proportions[i];
		console.log(`The letter ${ letter.letter } appears ${ (proportion * 100).toFixed(2) }% of the time`);
	}

	for (let i = 0; i < alphabet.length; i++) {
		const letter = alphabet[i];

		if (!letter.replacement) {
			letter.replacement = "-";
		}

		// We don't use Regex `.replace()` because letters that get replaced earlier
		// which are later selected for replacement will be overwritten
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