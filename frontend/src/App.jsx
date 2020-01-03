import React, { PureComponent } from "react";
import TraceList from "./TraceList";
import RegisterTable from "./RegisterTable";
import MemoryTable from "./MemoryTable";
import Api from "./Api";
import "./style.css";

let items = {};
let requestCache = {};

let arrowUp = 38
let arrowDown = 40

class App extends PureComponent {

    constructor(props) {
        super(props)
        this.traceList = null
        this.state = {
            items: {},
            requestCache: {},
            amountOfItems: 1000,
            selectedIndex: null,
            selectedItem: null,
            currentCommit: null,
            currentRegisters: null,
            currentMemory: null
        }

        Api.registerCommitHandler((data) => {
            let json = JSON.parse(data)
            console.log(json)
            let commits = json.commits
            let items = this.state.items
            commits.forEach((commit) => {
                if (commit.index) {
                    items[commit.index-1] = commit
                }
            })
            let itemCount = this.state.amountOfItems
            if (json.hasMore == false) {
                itemCount = json.commits.length
            }
            this.setState({
                items: items,
                amountOfItems: itemCount
            })
        })
        Api.registerRegisterHandler((data) => {
            let json = JSON.parse(data)
            let registers = json.registers
            this.setState({
                currentRegisters: registers,
                currentCommit: this.state.items[this.state.selectedIndex]
            })
        })
        Api.registerMemoryDumpHandler((data) => {
            let json = JSON.parse(data)
            let memory = json.memory
            this.setState({
                currentMemory: memory,
                memoryIndex: this.state.selectedIndex
            })
        })
    }

    isItemLoaded = ({ index }) => !!items[index];

    loadMoreTraces = (visibleStartIndex, visibleStopIndex) => {
        const key = [visibleStartIndex, visibleStopIndex].join(":"); // 0:10
        if (this.state.requestCache[key]) {
            return;
        }

        const length = visibleStopIndex - visibleStartIndex;
        const visibleRange = [...Array(length).keys()].map(
            x => x + visibleStartIndex
        );
        const itemsRetrieved = visibleRange.every(index => !!this.state.items[index]);

        if (itemsRetrieved) {
            requestCache = this.state.requestCache
            requestCache[key] = key;
            this.setState({
                requestCache: requestCache
            })
            return;
        }

        return Api.getCommits(visibleStartIndex, length, "next", false)
    };

    moveSelection = (offset) => {
        let currentIndex = this.state.selectedIndex
        let amountOfItems = this.state.amountOfItems
        if (currentIndex && items) {
            let newIndex = currentIndex + offset
            if (newIndex > 0 && newIndex <= amountOfItems) {
                this.onSelect(newIndex)
            }
        }
    }

    onSelect = (index) => {
        this.setState({
            selectedIndex: index
        })
        if (this.traceList) {
            this.traceList.current.scrollToItem(index-1)
        }
        Api.getRegisters(index)
        Api.getMemoryDump()
    }

    handleKeyDown = (event) => {
        if (event.keyCode == arrowUp) {
            this.moveSelection(-1)
            event.preventDefault()
        }
        if (event.keyCode == arrowDown) {
            this.moveSelection(1)
            event.preventDefault()
        }
    }

    componentDidMount() {
        document.addEventListener("keydown", this.handleKeyDown);
    }

    componentWillUnmount() {
        document.removeEventListener("keydown", this.handleKeyDown);
    }

  render() {
    return (
        <div style={{display: 'flex', flex: '1', flexDirection: 'row', height: "100%"}}>
          <div style={{flex: '1'}}>
              <TraceList
                  selectedIndex={this.state.selectedIndex}
                  onSelect={this.onSelect}
                  items={this.state.items}
                  itemCount={this.state.amountOfItems}
                  isItemLoaded={this.isItemLoaded}
                  loadMoreItems={this.loadMoreTraces}
                  selectItem={this.onSelect}
                  keyPressed={this.handleKeyDown}
                  onRef={ref => (this.traceList = ref)}
              />
          </div>
          <div style={{width: 350}}>
              <MemoryTable
                  currentMemory={this.state.currentMemory}
                  currentIndex={this.state.memoryIndex}
                  commits={this.state.items}
                />
          </div>
          <div style={{width: 350}}>
              <RegisterTable
                  currentCommit={this.state.currentCommit}
                  currentRegisters={this.state.currentRegisters}
                />
          </div>
        </div>
    );
  }
}

export default App;
