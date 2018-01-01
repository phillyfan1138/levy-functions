---
title: API Reference

language_tabs:
  - bash
  - javascript

search: true
---

# Introduction

This is the API for getting call, put, density, and implied volatility for an extended CGMY process with stochastic time change.  See the following papers for more information about this process.

* [Time Changed Levy Processes and Option Pricing](http://engineering.nyu.edu/files/jfetchgepaper.pdf)
* [Option Pricing Using Fourier Space Time-Stepping Framework](https://tspace.library.utoronto.ca/bitstream/1807/19300/1/Surkov_Vladimir_200911_PhD_Thesis.pdf)
* [Option Valuation Using the Fast Fourier Transform](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.348.4044&rep=rep1&type=pdf)
* [The Fine Structure of Asset Returns: An Empirical Investigation](http://finance.martinsewell.com/stylized-facts/distribution/CarrGemanMadanYor2002.pdf)
* [Specification Analysis of Option Pricing Models Based on Time-Changed Levy Processes](https://pdfs.semanticscholar.org/67cd/b553e2624c79a960ff79d0dfe6e6833690a7.pdf)
* [A Novel Pricing Method for European Options Based On Fourier-Cosine Series Expansions](http://ta.twi.tudelft.nl/mf/users/oosterle/oosterlee/COS.pdf)
* [A Fast and Accurate FFT-Based Method For Pricing Early-Exercise Options Under Levy Processes](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.377.1143&rep=rep1&type=pdf)

### Explanation of algorithms

This endpoint returns call option prices for an underlying that follows an extended CGMY process.  There are three algorithms to choose from: "fangoost", the algorithm proposed by Fang and Oosterlee; "carrmadan", the FFT based method proposed by Carr and Madan; and "fsts", a general FFT method proposed by Vladimir (and concurrently proposed by Fang, Oosterlee, Lord, and Bervoets).  These three algorithms each have benefits and tradeoffs.  

* FSTS: 
    * Benefits: It only requires a CF and a payoff. No further manipulations necessary. Can also price American options. 
    * Disadvantages: Prices in **O(n log n)** for Europeans and **O( m n log n)** for Americans.  Roughly twice as slow as Carr and Madan for Europeans. Prices in log asset instead of log strike; so not as easy to price all strikes for a given stock price.
    * Use in the API: The domain is based on the volatility of the underlying using sensible defaults. Ignores the "k" parameter in the JSON object.
* Carr Madan
    * Benefits: Probably the most commonly used method.  Prices in log strike so multiple options can be priced for a single asset price.
    * Disadvantages: Requires algebraic manipulations for each payoff.  Does not converge quickly.  
    * Use in the API: The domain is based on the volatility of the underlying using sensible defaults. Ignores the "k" parameter in the JSON object.
* Fang Oosterlee
    * Benefits: Unlike FFT based methods, the complex domain and the real domain can have different number of discrete steps.  Can price only the options available in the market without interpolation.  Can price in either the log asset or the log strike.  
    * Disadvantages: Requires algebraic manipulations for each payoff.  For a given number of discrete steps in the real domain is slower than Carr Madan.  However, typically there are far fewer discrete steps in the real domain.
    * Use in the API: The domain is set using the "k" parameter in the JSON object.  Every number in the "k" parameter is a strike to price the option at.

# Call

```bash
curl -X POST "https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/fangoost"
```

```javascript
const body={
    numU:6,/*gets raised to power of 2: 2^numU*/
    r:.03,
    T:.25,
    S0:50,
    sigma:.2,
    C:1.0,
    G:1.4,
    M:2.5,
    Y:.6,
    speed:.4,
    v0:1.05,
    adaV:.2,
    rho:-.5,
    k:[1.0, 1.5],
    quantile:.01
}
fetch('https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/fangoost', {
    method:'post',
    body
}).then(response=>response.json())
```

> The above command returns JSON structured like this:

```json
[
  {
    "atPoint": 1.0,
    "value": 5.0,
    "iv":0.23
  },
  {
    "atPoint": 1.5,
    "value": 4.5,
    "iv":0.2
  }
]
```

Retrieves call prices, asset/strike prices, and implied volatilities

### HTTP Request

`POST https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/<ALGORITHM>`

### URL Parameters

Parameter | Description
--------- | -----------
ALGORITHM | The option pricing algorithm to use.  Options are "fangoost", "carrmadan", and "fsts".  


# Put

```bash
curl -X POST "https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/fangoost"
```

```javascript
const body={
    numU:6,/*gets raised to power of 2: 2^numU*/
    r:.03,
    T:.25,
    S0:50,
    sigma:.2,
    C:1.0,
    G:1.4,
    M:2.5,
    Y:.6,
    speed:.4,
    v0:1.05,
    adaV:.2,
    rho:-.5,
    k:[1.0, 1.5],
    quantile:.01
}
fetch('https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/fangoost', {
    method:'post',
    body
}).then(response=>response.json())
```

> The above command returns JSON structured like this:

```json
[
  {
    "atPoint": 1.0,
    "value": 5.0
  },
  {
    "atPoint": 1.5,
    "value": 4.5
  }
]
```

Retrieves put prices, asset/strike prices, and implied volatilities

### HTTP Request

`POST https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/<ALGORITHM>`

### URL Parameters

Parameter | Description
--------- | -----------
ALGORITHM | The option pricing algorithm to use.  Options are "fangoost", "carrmadan", and "fsts".  

# Density

## Raw

```bash
curl -X POST "https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/raw"
```

```javascript
const body={
    numU:6,/*gets raised to power of 2: 2^numU*/
    r:.03,
    T:.25,
    S0:50,
    sigma:.2,
    C:1.0,
    G:1.4,
    M:2.5,
    Y:.6,
    speed:.4,
    v0:1.05,
    adaV:.2,
    rho:-.5,
    quantile:.01
}
fetch('https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/raw', {
    method:'post',
    body
}).then(response=>response.json())
```

> The above command returns JSON structured like this:

```json
[
  {
    "atPoint": 1.0,
    "value": 5.0
  },
  {
    "atPoint": 1.5,
    "value": 4.5
  }
]
```

Retrieves density of the CGMY process

### HTTP Request

`POST https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/raw`

## VaR

```bash
curl -X POST "https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/raw"
```

```javascript
const body={
    numU:6,/*gets raised to power of 2: 2^numU*/
    r:.03,
    T:.25,
    S0:50,
    sigma:.2,
    C:1.0,
    G:1.4,
    M:2.5,
    Y:.6,
    speed:.4,
    v0:1.05,
    adaV:.2,
    rho:-.5,
    quantile:.01
}
fetch('https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/var', {
    method:'post',
    body
}).then(response=>response.json())
```

> The above command returns JSON structured like this:

```json
{
    "ES": 2.0,
    "VaR": 1.5
}
```

Retrieves value at risk and expected shortfall for the CGMY process.

### HTTP Request

`POST https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/var`