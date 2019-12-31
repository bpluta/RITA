import React from 'react';

export default class RegisterRow extends React.Component {
  render() {
    return(
      <div className={this.props.isSelected ? "RegisterSelectedRow" : "RegisterRow"}>
          <div style={{flex: '1'}}>
              {this.props.register}
          </div>
          <div style={{flex: '1'}}>
              {this.props.value}
          </div>
      </div>
    )
  }
}
