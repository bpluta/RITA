import React from "react";
import { FixedSizeList as List } from "react-window";
import AutoSizer from "react-virtualized-auto-sizer";
import InfiniteLoader from "react-window-infinite-loader";
import TraceRow from "./TraceRow";

export default class TraceList extends React.Component {
    constructor(props) {
        super(props)
        this.listReference = React.createRef();
        this.state = {
            items: props.items,
            selectedIndex: props.selectedIndex,
            onSelect: props.onSelect
        }
    }

    Row = ({ index, style }) => {
      const item = this.state.items[index];
      let address
      if (item) {
          if (item.registers) {
              for (var i=0; i<item.registers.length; i++) {
                  if (item.registers[i].register == "RIP") {
                      address = item.registers[i].value
                  }
              }
          }
      }

      return (
          <div className={(index + 1) == this.props.selectedIndex ? "ListSelectedItem" : "ListItem"}
                style={style}
                onClick={ () => this.state.onSelect(index + 1) }
            >
            <TraceRow index={index + 1} address={address ? address : "Loading..."} content={item ? `${item.instruction.value}` : "Loading..."}/>
          </div>
      )
    }

    render() {
        return (
                <AutoSizer>
                  {({ height, width }) => (
                    <InfiniteLoader
                      isItemLoaded={this.props.isItemLoaded}
                      loadMoreItems={this.props.loadMoreItems}
                      itemCount={this.props.itemCount}
                    >
                      {({ onItemsRendered, ref }) => {
                          this.props.onRef(this.listReference)
                          return (
                              <List
                                className="List"
                                height={height}
                                itemCount={this.props.itemCount}
                                itemSize={25}
                                width={width}
                                ref={this.listReference}
                                onItemsRendered={onItemsRendered}
                                selectedIndex={this.props.selectedIndex}
                              >
                                {this.Row}
                              </List>
                          )

                      }}
                    </InfiniteLoader>
                  )}
                </AutoSizer>
        )
    }
}
