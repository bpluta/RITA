import React from 'react';

export default class TraceRow extends React.Component {

    render() {
        return(
            <div style={{display: 'flex', flex: '1', flexDirection: 'row', alignItems: "center", justifyContent: "center"}}>
                <div className={"TraceColumn"} style={{width: 50}}>
                    {this.props.index}
                </div>
                <div className={"TraceColumn"} style={{width: 120}}>
                    {this.props.address}
                </div>
                <div className={"TraceColumn"} style={{flex: '1'}}>
                    {this.props.content}
                </div>
            </div>
        )
    }
}
