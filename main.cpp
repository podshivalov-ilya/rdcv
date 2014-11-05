#include <iostream>
#include <vector>
#include <string>

#include <cmath>

#include <shark/Data/Dataset.h>
#include <shark/Data/Csv.h>

using shark::LabeledData;
using shark::RealVector;
using shark::RealMatrix;

LabeledData<RealVector, RealVector> loadData()
{
    shark::Data<RealVector> data;
    shark::Data<RealVector> labels;

    std::string dataFile = "./data/descriptors.csv";
    std::string labelsFile = "./data/mtp.csv";

    try{
        shark::importCSV(data, dataFile, ' ', '#');
        shark::importCSV(labels, labelsFile, ' ', '#');
    } catch (...) {
        std::cerr << "Unable to open file " <<  dataFile << " and/or " << labelsFile << ". Check paths!" << std::endl;
        exit(EXIT_FAILURE);
    }
    LabeledData<RealVector, RealVector> dataset(data, labels);
    return dataset;
}

int echoArray(std::vector<std::vector<double> *> arr, std::string pre = "")
{
    std::vector<std::vector<double> *>::iterator ul;
    for(ul = arr.begin(); ul != arr.end(); ul++)
    {
        std::vector<double>::iterator dl;
        std::cout << pre;
        for(dl = (*ul)->begin(); dl != (*ul)->end(); dl++)
            std::cout << *dl << "\t";
        std::cout << std::endl;
    }
}

int echoArrayP(std::vector<std::vector<double> *> arr, std::string pre = "")
{
    std::cout << pre;
    for(int ul = 0; ul < arr.size(); ul++)
        std::cout << arr[ul] << "\t";
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    int n = 4;
    int seg_test = 4;
    int seg_calib= 4;

    /* Maybe vector of pointers to vector ? */
    std::vector<std::vector<double>* > x_in;
    std::vector<std::vector<double>* > y_in;

    /*Initializing dataset*/
    for(int i = 0; i < 12; i++) {
        std::vector<double> *x_i = new std::vector<double>;
        std::vector<double> *y_i = new std::vector<double>;

        double a = i + 0.1;

        double sum = 0;
        double p = 1;
        for(int j = 0; j < 10; j++) {
            double b = a * pow(-1.0, j) + j * pow(-1.0, j+1);
            x_i->push_back(b);
            sum += b;
            p *= b;
        }
        y_i->push_back(sum);
        y_i->push_back(p);
        y_i->push_back(p - sum);

        x_in.push_back(x_i);
        y_in.push_back(y_i);
    }

    /*Splitting dataset*/

    std::cout << "==========X==========\n";
    echoArray(x_in);
    //std::cout << "==========Y==========\n";
    //echoArrayP(y_in);

    /* Runing through the vectors dataset */
    /* There is now sequential but rewrite for random */
    for(int tau = 0; tau < n - 1; tau++) {
        /* Setting range of test set */
        int test_set_begin = tau * n;
        int test_set_end = (tau + 1) * n - 1;

        /* Setting test set and validation set */
        std::vector<std::vector<double> *> test_set_x;
        std::vector<std::vector<double> *> test_set_y;

        std::vector<std::vector<double> *> clb_set_x;
        std::vector<std::vector<double> *> clb_set_y;

        for(int clb = 0; clb < test_set_begin; clb++) {
            clb_set_x.push_back( x_in[clb] );
            clb_set_y.push_back( y_in[clb] );
        }
        for(int clb = test_set_begin; clb <= test_set_end; clb++) {
            test_set_x.push_back( x_in[clb] );
            test_set_y.push_back( y_in[clb] );
        }
        for(int clb = test_set_end + 1; clb < x_in.size(); clb++) {
            clb_set_x.push_back( x_in[clb] );
            clb_set_y.push_back( y_in[clb] );
        }
        std::cout << "==========X" << tau << "_test==========\n";
        echoArray(test_set_x, "\t");
        //std::cout << "==========Y" << tau << "_test==========\n";
        //echoArray(test_set_y, "\t");

        std::cout << "==========X" << tau << "_clb==========\n";
        echoArray(clb_set_x, "\t");
        //std::cout << "==========Y" << tau << "_clb==========\n";
        //echoArray(clb_set_y, "\t");
    }

    /* Destruct datasets */
    for(int del = 0; del < x_in.size(); del++)
    {
        delete x_in[del];
        delete y_in[del];
    }
    return 0;
}
