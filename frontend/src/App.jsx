import React, { PureComponent } from "react";
import TraceList from "./TraceList";
import Api from "./Api";
import "./style.css";

let items = {};
let requestCache = {};

class App extends PureComponent {

    constructor(props) {
        super(props)
        this.state = {
            items: {},
            requestCache: {},
            amountOfItems: 1000
        }

        Api.registerCommitHandler((data) => {
            let json = JSON.parse(data)
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

  render() {
    return (
        <TraceList
            items={this.state.items}
            itemCount={this.state.amountOfItems}
            isItemLoaded={this.isItemLoaded}
            loadMoreItems={this.loadMoreTraces}
        />
    );
  }
}

export default App;
