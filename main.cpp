#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "get_cf.h"
#include "parse_json.h"

const int carrmadanput=0;
const int carrmadancall=1;
const int fangoostput=2;
const int fangoostcall=3;
const int fstsput=4;
const int fstscall=5;
const int VaR=6;
const int density=7;



template<typename CF>
void carr_madan_put(const CF& cf, int numU, double discount, double S0, double xMax){
    auto ada=M_PI/xMax;//.25;
    json_print_options(optionprice::CarrMadanPut(
        numU,  
        ada,
        S0, 
        discount, 
        cf
    ), optionprice::getCarrMadanStrikes(
        ada, S0, numU
    ));
}
template<typename CF>
void carr_madan_call(const CF& cf, int numU, double discount, double S0, double xMax){
    auto ada=M_PI/xMax;//.25;
    json_print_options(optionprice::CarrMadanCall(
        numU,  
        ada,
        S0, 
        discount, 
        cf
    ), optionprice::getCarrMadanStrikes(
        ada, S0, numU
    ));
}
template<typename CF, typename ParsedJ>
void fsts_call(const CF& cf, const ParsedJ& parsedJson, int numU, double discount, double xMax){
    auto K=get_k_var(parsedJson).k[0];//only one
    json_print_options(optionprice::FSTS(
        numU, 
        xMax, 
        [&](const auto& x){
            return discount;
        },
        [&](
            const auto& logR
        ){
            auto assetValue=K*exp(logR);
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    ), optionprice::getFSTSUnderlying(
        -xMax, xMax, K, numU
    ));
}
template<typename CF, typename ParsedJ>
void fsts_put(const CF& cf, const ParsedJ& parsedJson, int numU, double discount, double xMax){
    auto K=get_k_var(parsedJson).k[0];//only one
    json_print_options(optionprice::FSTS(
        numU,  
        xMax,
        [&](const auto& x){
            return discount;
        },
        [&](
            const auto& logR
        ){
            auto assetValue=K*exp(logR);
            return assetValue<K?K-assetValue:0.0;
        },
        cf
    ), optionprice::getFSTSUnderlying(
        -xMax, xMax, K, numU
    ));
}

template<typename CF, typename ParsedJ>
void fangoost_call(const CF& cf, const ParsedJ& parsedJson, int numU, double discount, double S0, double xMax){

    auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_options(optionprice::FangOostCall(
        S0, strikes,
        numU,  
        discount,
        cf
    ), strikes);
}
template<typename CF, typename ParsedJ>
void fangoost_put(const CF& cf, const ParsedJ& parsedJson, int numU, double discount, double S0, double xMax){
    auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_options(optionprice::FangOostPut(
        S0, strikes,
        numU,  
        discount,
        cf
    ), strikes);
}
template<typename CF, typename ParsedJ>
void get_var(const CF& cf, const ParsedJ& parsedJson, int numU, double xMax){
    auto dist_var=get_dist_variables(parsedJson);
    auto prec=.0000001;
    auto var=cfdistutilities::computeVaR(dist_var.quantile, prec, -xMax, xMax, numU, cf);
    auto ES=cfdistutilities::computeES(
        dist_var.quantile,
        -xMax, xMax, var, numU, cf, true
    );
    json_print_var(var, ES);
}
template<typename CF>
void get_density(const CF& cf, int numU, double xMax){
    int numX=128;
    json_print_options(
        fangoost::computeInv(
            numX, numU, -xMax, xMax, cf
        ), 
        fangoost::computeXRange(numX, -xMax, xMax)
    );
}

int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto options=get_option_var(parsedJson);
        
        auto cgmyCF=cf(
            options.r,
            options.sigma,
            options.T,
            options.S0,
            options.C,
            options.G,
            options.M,
            options.Y,
            options.speed,
            options.v0,
            options.adaV,
            options.rho
        );
        double discount=exp(-options.r*options.T);
        /**NOTE that this is a big assumption about the
         * domain for these distributions.
         * Be careful!*/
        double xMax=sqrt(options.T)*5.0;
        int numU=pow(2, options.numU);
        int key=std::stoi(argv[1]);
        switch(key){
            case carrmadanput: {
                carr_madan_put(
                    cgmyCF, numU, 
                    discount, options.S0,
                     xMax
                ); 
                break;
            }
            case carrmadancall: {
                carr_madan_call(
                    cgmyCF, numU, 
                    discount, options.S0,
                     xMax
                ); 
                break;
            }
            case fstscall: {
                fsts_call(
                    cgmyCF, parsedJson, 
                    numU, discount,
                    xMax
                );
                break;
            }
            case fstsput: {
                fsts_put(
                    cgmyCF, parsedJson, 
                    numU, discount,
                    xMax
                );
                break;
            }
            case fangoostcall: {
                fangoost_call(
                    cgmyCF, parsedJson, 
                    numU, discount, 
                    options.S0,
                     xMax);
                break;
            }
            case fangoostput: {
                fangoost_put(
                    cgmyCF, parsedJson, 
                    numU, discount,
                    options.S0,
                     xMax);
                break;
            }
            case VaR: {
                get_var(
                    cgmyCF, parsedJson, 
                    numU, xMax
                );
                break;
            }
            case density: {
                get_density(
                    cgmyCF, numU, 
                     xMax
                );
                break;
            }
        }



    }


    return 0;
}