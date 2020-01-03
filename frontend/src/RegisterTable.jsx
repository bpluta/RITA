import React from 'react';
import { FixedSizeList as List } from "react-window";
import AutoSizer from "react-virtualized-auto-sizer";
import RegisterRow from './RegisterRow';

const registerNames = ["RAX", "RBX", "RCX", "RDX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS", "CS", "FS", "GS"]

export default class RegisterTable extends React.Component {
    constructor(props) {
        super(props)

        this.state = {
            currentCommit: props.currentCommit,
            currentRegisters: props.currentRegisters,
            onSelect: props.onSelect
        }
    }

    Row = ({ index, style }) => {
      let registers = this.props.currentRegisters
      let currentCommit = this.props.currentCommit

      let register = registerNames[index]
      let value = "0x0"

      if (registers != null) {
          if (registers[register]) {
              value = registers[register]
          }
      }
      let isSelected = false
      if (currentCommit != null) {
          for (var i=0; i<currentCommit.registers.length; i++) {
              let registerCommit = currentCommit.registers[i]
              if (registerCommit.register == register) {
                  isSelected = true
                  break
              }
          }
      }
      return (
          <div style={style}>
            <RegisterRow
                isSelected={isSelected}
                register={register}
                value={value}
            />
          </div>
      )
    };

    render() {
      return(
        <AutoSizer>
          {({ height, width }) => (
              <List
                className="List"
                height={height}
                itemCount={registerNames.length}
                itemSize={35}
                width={width}
                currentRegisters={this.props.currentRegisters}
              >
                {this.Row}
              </List>
          )}
        </AutoSizer>
      )
    }
}
