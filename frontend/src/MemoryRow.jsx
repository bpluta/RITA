import React from 'react';

export default class MemoryRow extends React.Component {
  render() {
    return(
      <div className={this.props.isSelected ? "RegisterSelectedRow" : "RegisterRow"}>
          <div className={"TraceColumn"} style={{flex: '1'}}>
              {this.props.address}
          </div>
          <div className={"TraceColumn"} style={{flex: '1'}}>
              {this.props.value}
          </div>
      </div>
    )
  }
}
