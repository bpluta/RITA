import React from 'react';
import RegisterRow from './RegisterRow';

const registerNames = ["RAX", "RBX", "RCX", "RDX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS", "CS", "FS", "GS"]

export default class RegisterTable extends React.Component {
    render() {
        let items = []
        registerNames.forEach((register) => {
            let value = "0x0"
            let registers = this.props.registers
            if (registers != null) {
                if (Object.keys(registers).length === 0 && registers.constructor === Object) {
                    if (register in registers) {
                        value = registers[register]
                    }
                }
            }
            items.push(
                <RegisterRow
                    register={register}
                    value={value}
                />
            )
        })

        return (
            <div>{items}</div>
        )
    }
}
