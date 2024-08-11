function make_environment(...envs) {
    return new Proxy(envs, {
        get(target, prop, receiver) {
            for (let env of envs) {
                if (env.hasOwnProperty(prop)) {
                    return env[prop];
                }
            }
            return (...args) => { console.error("Not implemented: " *prop, args) };
        }
    });
}

function console_panic() {
    alert("Invalid grid");
}

WebAssembly.instantiateStreaming(fetch("./wasm.wasm"), {
    "env": make_environment({
        "console_panic": console_panic,
    }),
}).then(w0 => {
    function focusNext(num) {
        const currInput = document.activeElement;
        const currInputIndex = cells.indexOf(currInput);
        const nextinputIndex =
            (currInputIndex + 1) % cells.length;
        const cell = cells[nextinputIndex];
        if (num === " ") {
            //cell.value = " ";
        }
        else {
            cell.value = num;
        }
        cell.focus();
    }
    
    let sudoku = document.getElementById("sudoku_grid");
    let cell = (i) => `<input id="cell${i}" class="sudoku-cell" maxlength=1 type="text"  pattern="[1-9]*" inputmode="numeric"/>`;
    let grid = ""
    for (i = 0; i < 81; i++) {
        grid += cell(i)
    }
    sudoku.innerHTML = grid;
    
    let cells = [];
    for (i = 0; i < 81; i++) {
        cells[i] = document.getElementById(`cell${i}`);
    }
    console.log(cells);
    cells.forEach((cell, i, cells) => {
        cell.addEventListener("keydown", (event) => {
            const num = Number(event.key);
            if (event.key === " ") {
                if (cell.value.length >= cell.maxLength) {
                    event.preventDefault();
                    focusNext(" ");
                }
            }
            else if ((num || num == 0) && num >= 0 && num <= 9) { // Only allow numbers
                if (cell.value.length >= cell.maxLength) {
                    event.preventDefault();
                    focusNext(num);
                }
            }
            else if (event.key === 'Backspace') {
                if (cell.value === '' && i - 1 >= 0) {
                    cells[(cells.indexOf(cell) - 1) % cells.length].focus();
                    event.preventDefault();
                }
            }
            else if (event.key === "ArrowLeft" && i - 1 >= 0) {
                cells[(cells.indexOf(cell) - 1) % cells.length].focus();
                event.preventDefault();
            }
            else if (event.key === "ArrowRight" && i + 1 < cells.length) {
                cells[(cells.indexOf(cell) + 1) % cells.length].focus();
                event.preventDefault();
            }
            else if (event.key === "ArrowUp" && i - 9 >= 0) {
                cells[(cells.indexOf(cell) - 9) % cells.length].focus();
                event.preventDefault();
            }
            else if (event.key === "ArrowDown" && i + 9 < cells.length) {
                cells[(cells.indexOf(cell) + 9) % cells.length].focus();
                event.preventDefault();
            }
            else if (event.key === "Tab") {
            }
            else {
                event.preventDefault();
            }
        });
    });
    
    let button = document.getElementById("submit-button");
    button.addEventListener("click", (event) => {
        console.log(w0);
        const w0buffer = w0.instance.exports.memory.buffer;
        const wgrid = w0.instance.exports.grid;
        
        for (i = 0; i < 81; i++) {
            w0.instance.exports.set_value(i, cells[i].value);
        }
        let solved = w0.instance.exports.solve(wgrid);
        console.log(solved);
        if (solved) {
            let values2 = new Uint8Array(w0buffer, wgrid, 81);
            console.log(values2);
            for(i = 0; i < 81; i++) {
                cells[i].value = values2[i];
            }
            let result = document.getElementById("result");
            result.innerHTML = "Solved.";
            result.style.backgroundColor = "#A0F0A0"
        }
        else {
            let result = document.getElementById("result");
            result.innerHTML = "Unsolvable.";
            result.style.backgroundColor = "#F0A0A0"
        }
    });
});
