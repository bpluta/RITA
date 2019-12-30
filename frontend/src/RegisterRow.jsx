import React from 'react';

export default class RegisterRow extends React.Component {
  render() {
    return(
      <div className={"RegisterRow"}>
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
