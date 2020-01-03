import React from 'react';
import { FixedSizeList as List } from "react-window";
import AutoSizer from "react-virtualized-auto-sizer";
import MemoryRow from './MemoryRow';

let currentMemory = null

export default class MemoryTable extends React.Component {
    constructor(props) {
        super(props)

        this.state = {
            currentCommit: props.currentCommit,
            currentRegisters: props.currentRegisters,
            onSelect: props.onSelect,
        }
    }

    Row = ({ index, style }) => {
        let memoryDump = currentMemory
        let address = Object.keys(memoryDump)[index]
        let currentIndex = this.props.currentIndex
        let commits = this.props.commits
        let currentValue = ""
        let isSelected = false
        if (address) {
            let values = memoryDump[address]
            currentValue = values[values.length-1].value
        } else {
            address = ""
        }
        if (currentIndex != null && commits != null) {
            let commit = commits[currentIndex-1]
            if (commit) {
                let commitedMemory = commit.memory
                if (commitedMemory) {
                    for (var i=0; i<commitedMemory.length; i++) {
                        if (commitedMemory[i].address == address) {
                            isSelected = true
                            break
                        }
                    }
                }
            }
        }

      return (
          <div style={style}>
            <MemoryRow
                isSelected={isSelected}
                address={address}
                value={currentValue}
            />
          </div>
      )
    };

    render() {
        let itemCount = 0
        currentMemory = {}
        if (this.props.currentMemory && this.props.currentMemory != null) {
            let currentIndex = this.props.currentIndex
            if (currentIndex) {
                for (var key in this.props.currentMemory) {
                    let commits = this.props.currentMemory[key]
                    let memoryCommit = []
                    for (var i = 0; i<commits.length; i++) {
                        let commit = commits[i]
                        if (commit.index > currentIndex) break
                        memoryCommit.push(commit)
                    }
                    if (memoryCommit.length > 0) {
                        currentMemory[key] = memoryCommit
                        itemCount += 1
                    }
                }
            }
        }
      return(
        <AutoSizer>
          {({ height, width }) => (
              <List
                className="List"
                height={height}
                itemCount={itemCount}
                itemSize={35}
                width={width}
                currentMemory={this.props.currentMemory}
              >
                {this.Row}
              </List>
          )}
        </AutoSizer>
      )
    }
}
