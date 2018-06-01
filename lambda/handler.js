'use strict'
const {spawn} = require('child_process')
const https = require('https')

const calculatorKeys={
  putpricecarrmadan:0,
  callpricecarrmadan:1,
  putpricefangoost:2,
  putdeltafangoost:3,
  putthetafangoost:4,
  putgammafangoost:5,
  callpricefangoost:6,
  calldeltafangoost:7,
  callthetafangoost:8,
  callgammafangoost:9,
  putpricefsts:10,
  putdeltafsts:11,
  putthetafsts:12,
  putgammafsts:13,
  callpricefsts:14,
  calldeltafsts:15,
  callthetafsts:16,
  callgammafsts:17,
  densityvar:18,
  densityraw:19
}
const calibratorKeys={
  spline:0,
  calibrate:1
}

const totalKeys=[
  "lambda", 
  "muJ",
  "sigJ",
  "v0",
  "sigma",
  "rho",
  "adaV",
  "speed"
]

const calibratorRequiredKeys=body=>{
  const totalKey=Object.assign({}, body, body.variable)
  return totalKeys.find(key=>totalKey[key]===undefined)
}

process.env['PATH']=`${process.env['PATH']}:${process.env['LAMBDA_TASK_ROOT']}`

const gMsg=statusCode=>body=>({
  statusCode,
  headers: {
    "Access-Control-Allow-Origin" : "*", // Required for CORS support to work
    "Access-Control-Allow-Credentials" : true, // Required for cookies, authorization headers with HTTPS 
    'Content-Type': 'application/json',
  },
  body
})
const errMsg=gMsg('400')
const msg=gMsg('200')


const genericSpawn=(binary, options, callback)=>{
  const binaryPath=process.env['LAMBDA_TASK_ROOT']?`${__dirname}/${binary}`:`./bin/${binary}`
  const model=spawn(binaryPath,options)
  let modelOutput=''
  let modelErr=''
  model.stdout.on('data', data=>{
    modelOutput+=data
  })
  model.stderr.on('data', data=>{
    modelErr+=data
  })
  model.on('close', code=>{
    if(modelErr){
      return callback(null, errMsg(modelErr))
    }
    return callback(null, msg(modelOutput))
  })
}
const getParametersOrObject=parameters=>parameters||"{}"
const spawnBinary=binary=>(functionalityIndicator, parms, callback)=>{
  genericSpawn(binary, [functionalityIndicator,getParametersOrObject(parms)], callback)
}

const calculatorSpawn=spawnBinary('calculator')
const calibratorSpawn=spawnBinary('calibrator')
const defaultParametersSpawn=callback=>genericSpawn('defaultParameters', [], callback)


module.exports.calculator=(event, context, callback)=>{
  const {optionType, sensitivity, algorithm}=event.pathParameters
  const key=optionType+sensitivity+algorithm
  const index=calculatorKeys[key]
  calculatorSpawn(index, event.body, callback)
}
module.exports.density=(event, context, callback)=>{
  const {densityType}=event.pathParameters
  const key='density'+densityType
  calculatorSpawn(calculatorKeys[key], event.body, callback)
}
module.exports.defaultParameters=(event, context, callback)=>{
  defaultParametersSpawn(callback)
}

module.exports.calibrator=(event, context, callback)=>{
  const {calibration}=event.pathParameters
  const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
  if(keyResult){
    const err=`Requires additional keys!  Missing ${keyResult}`
    return callback(null,  errMsg(err))
  }
  calibratorSpawn(calibratorKeys[calibration], event.body, callback)
}
module.exports.calculatorKeys=calculatorKeys


const removeIlliquidOptionPrices=optionData=>{
  
}
//returns object with S0, array of objects with strike, time to maturity, and price
//we WILL HAVE to update the cpp code to incorporate the effect.  But this will
//resolve issue 7
const filterOptionData=data=>{
  
}
module.exports.getOptionPrices=(event, context, callback)=>{
  const {asOfDate, ticker}=event.pathParameters
  //todo!  fix this to have a default of no as of date (ie, current)
  const query=`https://query1.finance.yahoo.com/v7/finance/options/${ticker}/${asOfDate}`

 
  https.get(query, resp => {
    let data = '';
  
    // A chunk of data has been recieved.
    resp.on('data', chunk => {
      data += chunk;
    })
  
    // The whole response has been received. Print out the result.
    resp.on('end', () => {
      filterOptionData(JSON.parse(data))
    })
  
  }).on('error', err => {
   // console.log("Error: " + err.message);
  });

}