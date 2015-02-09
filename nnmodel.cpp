#include "nnmodel.h"

//#define DEBUG

NNModel::NNModel()
{
    nAllData = 4;
    nCalibrationSet = 4;

    firstHiddenLayerLB = 0;
    firstHiddenLayerTB = 0;

    hiddenLayersLB = 0;
    hiddenLayersTB = 0;

    totalHiddenLayerLB = 0;
    totalHiddenLayerTB = 0;

    in_cnt = 0;
    preBM = false;
}

bool NNModel::setFirstHiddenLayerRange(unsigned int lb, unsigned int tb)
{
    if( tb <= lb )
        return false;

    firstHiddenLayerLB = lb;
    firstHiddenLayerTB = tb;

    return true;
}

bool NNModel::setOtherHiddenLayerRange(unsigned int lb, unsigned int tb)
{
    if( tb <= lb )
        return false;

    totalHiddenLayerLB = lb;
    totalHiddenLayerTB = tb;

    return true;
}

bool NNModel::setHiddenLayersRange(unsigned int lb, unsigned int tb)
{
    if( tb <= lb )
        return false;

    hiddenLayersLB = lb;
    hiddenLayersTB = tb;

    return true;
}

bool NNModel::checkParams()
{
    if(firstHiddenLayerLB > 0 && firstHiddenLayerTB > 0 &&
            hiddenLayersLB > 0 && hiddenLayersTB > 0 &&
            totalHiddenLayerLB > 0 && totalHiddenLayerTB > 0 &&
            in_cnt > 0){
        return true;
    }
    return false;
}

bool NNModel::loadData(std::string dataFileName, std::string labelsFileName)
{
    shark::Data<shark::RealVector> data;
    shark::Data<shark::RealVector> labels;

    try{
        shark::importCSV(labels, labelsFileName, ' ', '#');
        shark::importCSV(data, dataFileName, ' ', '#');
    } catch (shark::Exception e){
        std::cerr << "Exception!\n\t" << e.file() << "\t" << e.line() << "\t" << e.what() << std::endl;
        std::cerr << "Unable to open file " <<  dataFileName << " and/or " << labelsFileName << ". Check paths!" << std::endl;
        return false;
    }
    dataset = shark::LabeledData<shark::RealVector, shark::RealVector> (data, labels);

    in_cnt = shark::dataDimension(dataset.inputs());
    return true;
}

std::vector<std::vector<size_t> > NNModel::genMidLayers(unsigned beg, unsigned end, unsigned count)
{
    vector<vector<size_t> > lst;
    if(count == 1){
        for(int i = beg; i <= end; i++){
            vector<size_t> l;
            l.push_back(i);
            lst.push_back(l);
        }
    }
    else{
        vector<vector<size_t> > lst2 = genMidLayers(beg, end, count - 1);
        for(int i = beg; i <= end; i++){
            for(int j = 0; j < lst2.size(); j++){
                vector<size_t> l = lst2[j];
                l.push_back(i);
                lst.push_back(l);
            }
        }
    }
    return lst;
}

int NNModel::push_back_s(std::vector<size_t> *changeable, const std::vector<size_t> &items)
{
    std::vector<size_t>::const_iterator item;
    for(item = items.begin(); item != items.end(); item++){
        changeable->push_back(*item);
    }
}

//#ifdef DEBUG
int NNModel::echoArch(vector<size_t> arr, std::string pre)
{
    cerr << pre;
    for(int ul = 0; ul < arr.size(); ul++) {
        if(ul == arr.size() - 1)
            cerr << arr[ul];
        else
            cerr << arr[ul] << "->";
    }
    cerr << std::endl;
}
//#endif

void NNModel::setPreBM(bool nw)
{
    preBM = nw;
}

shark::BinaryRBM NNModel::trainRBM(
        shark::UnlabeledData<RealVector> const& data,//the data to train with
        std::size_t numHidden,//number of features in the AutoencoderModel
        std::size_t iterations, //number of iterations to optimize
        double regularisation,//strength of the regularisation
        double learningRate // learning rate of steepest descent
        )
{
    //create rbm with simple binary units using the global random number generator
    std::size_t inputs = dataDimension(data);
    shark::BinaryRBM rbm(shark::Rng::globalRng);
    rbm.setStructure(inputs,numHidden);
    initRandomUniform(rbm,-0.1*std::sqrt(1.0/inputs),0.1*std::sqrt(1.0/inputs));//initialize weights uniformly

    //create derivative to optimize the rbm
    //we want a simple vanilla CD-1.
    shark::BinaryCD estimator(&rbm);
    shark::TwoNormRegularizer regularizer;
    //0.0 is the regularization strength. 0.0 means no regularization. choose as >= 0.0
    estimator.setRegularizer(regularisation,&regularizer);
    estimator.setK(1);//number of sampling steps
    estimator.setData(data);//the data used for optimization

    //create and configure optimizer
    shark::SteepestDescent optimizer;
    optimizer.setLearningRate(learningRate);//learning rate of the algorithm

    //now we train the rbm and evaluate the mean negative log-likelihood at the end
    unsigned int numIterations = iterations;//iterations for training
    optimizer.init(estimator);
    for(unsigned int iteration = 0; iteration != numIterations; ++iteration) {
        optimizer.step(estimator);
    }
    rbm.setParameterVector(optimizer.solution().point);
    return rbm;
}

// I'll have modify it for any arctitecture!
Network NNModel::unsupervisedPreTraining(
        shark::UnlabeledData<RealVector> const& data,
        vector<size_t> lrs,
        double regularisation, std::size_t iterations, double learningRate
        )
{
    Network nw;
    nw.setStructure(lrs, shark::FFNetStructures::Normal, true);
    initRandomNormal(nw,0.1);

    shark::UnlabeledData<RealVector> intermediateData = data;
    vector<shark::BinaryRBM> pLayers;

    for(int lr = 1; lr < lrs.size() - 1; lr++) {
        //cerr << "pre-training " << lr << " layer of " << lrs.size() - 2 << " hidden" << std::endl;
        // loop layers array with condition for first layer pre-training
        shark::BinaryRBM l =  trainRBM(
                intermediateData, lrs[lr],
                regularisation,iterations, learningRate
                );

        if(lr == 1){
            //compute the mapping onto features of the first hidden layer
            l.evaluationType(true,true);//we compute the direction visible->hidden and want the features and no samples
        }
        intermediateData=l(intermediateData);
        pLayers.push_back(l);
    }
    for(int i = 0; i < pLayers.size() - 1; i++)
        nw.setLayer(i, pLayers[i].weightMatrix(),pLayers[i].hiddenNeurons().bias());
    //network = nw;
    return nw;
}

int NNModel::evaluate()
{
    if(!checkParams()){
        return CHECKPARAMS;
    }

#ifdef DEBUG
    cerr << "DS: " << dataset.numberOfElements() << std::endl;
#endif

    for(int fhl = firstHiddenLayerLB; fhl < firstHiddenLayerTB; fhl++){
        for(int hlCount = hiddenLayersLB; hlCount < hiddenLayersTB; hlCount++){
            if(hlCount > 0){
                vector<vector<size_t> > hiddenMidLayers = genMidLayers(totalHiddenLayerLB, totalHiddenLayerTB, hlCount);
                std::vector<std::vector<size_t> >::iterator l;
                for(l = hiddenMidLayers.begin(); l != hiddenMidLayers.end(); l++){
                    std::vector<size_t> layers;
                    // Setting up architecture of NN
                    layers.push_back(in_cnt);
                    layers.push_back(fhl);
                    push_back_s(&layers, *l);
                    layers.push_back(1);

                    //network.setStructure(layers, shark::FFNetStructures::Normal, true);
                    network = unsupervisedPreTraining(dataset.inputs(), layers, 0.001, 1000, 0.1);
                    // All prepared
#ifdef DEBUG
                    echoArch(layers);
#endif
                    dataset.shuffle();
                    LabeledData<RealVector, RealVector> testSet, calibrationSet;
                    LabeledData<RealVector, RealVector> validationSet, trainingSet;

                    //RealVector MSE;

                    RealVector opt = network.parameterVector();
                    bool was = false;

                    double bestError = DBL_MAX;
                    for(int k = 1; k < nCalibrationSet; k++){
#ifdef DEBUG
                        cerr << "==============================" << k << "==============================" << std::endl;
#endif
                        calibrationSet = dataset;

                        testSet = shark::splitAtElement(calibrationSet, static_cast<size_t>((1.0 - 1.0/nAllData) * calibrationSet.numberOfElements()));
                        validationSet = shark::splitAtElement(calibrationSet, static_cast<size_t>((1.0 - 1.0/nCalibrationSet) * calibrationSet.numberOfElements()));
                        trainingSet = calibrationSet;

                        shark::initRandomUniform(network, -0.3, 0.3);
                        shark::IRpropPlus optimizer;
                        shark::ErrorFunction<RealVector, RealVector> error(trainingSet, &network, &loss);
                        optimizer.init(error);

                        int validationOffset = 0;

                        shark::Data<RealVector> prediction;
                        int bestIteration = 0;

                        const double MaxIterationsFromMinimum = 90;

                        for(int i = 0; i < trainingSet.numberOfElements(); i++){
                            optimizer.step(error);
                            prediction = network(validationSet.inputs());
                            double e = loss.eval(validationSet.labels(), prediction);

                            if(bestError >= e) {
                                bestIteration = 0;
                                bestError = e;

                                was = true;
                                opt = network.parameterVector();
                            }

                            if(bestIteration > MaxIterationsFromMinimum) {
#ifdef DEBUG
                                cerr << "Break max iteration after minimum.\n";
#endif
                                break;
                            }

                            bestIteration++;
#ifdef DEBUG
                            cerr << "Iteration " << i << " error " << e << " " << bestError << std::endl;
#endif
                        }
                    }
                    if(was)
                        network.setParameterVector(opt);

                    // Section of calculate model parametres. Wrap to other function.

                    // X is observed value
                    // Y is calculated value
                    // 
                    //       1
                    // mse = n * SUM( X - Y )^2
                    double mse = 0; // Mean squared error
                    //             SUM( X - Y )^2
                    // q^2 = 1.0 - SUM( X_aver - X)^2
                    double qsq = 0;          // Coefficient of determination
                    double mvX = 0, mvY = 0; // Mid value of X and Y
                    vector<double> Ycal;     // Observed array
                    // delta = SUM( Y - X )^2
                    double deltaSum = 0;     // Sum of squared differ between calculated and observed. There is upper half of q^2
#ifdef DEBUG
                    cerr << "Y\tX\tdelta^2\n";
#endif
                    for(int i = 0; i < testSet.numberOfElements(); i++){
                        RealVector Y;
                        // getting i-ths observed value
                        double x = testSet.labels().element(i)[0];
                        // evaluate i-ths set to Y
                        network.eval(testSet.inputs().element(i), Y);
                        deltaSum += (Y(0) - x) * (Y(0) - x);
#ifdef DEBUG
                        cerr << Y(0) << '\t' << x << '\t' << (Y(0) - x)*(Y(0) - x) << std::endl;
#endif
                        mse += (Y(0) - x)*(Y(0) - x);

                        mvX += x;
                        mvY += Y(0);
                        Ycal.push_back(Y(0));
                    }
                    mse = mse / testSet.numberOfElements();
                    mvX /= testSet.numberOfElements();  // X average divides by N (there is for averaging. Lol)
                    mvY /= testSet.numberOfElements();  // Y average divides by N (there is for averaging. Lol)

                    //        SUM( X - Xaver) * (Y - Yaver )
                    // corr = sqrt(SUM( Xaver - X )^2 * SUM( Yaver - Y )^2 )
                    double corr = 0;

                    // covXY = SUM( X - Xaver ) * (Y - Yaver )
                    double covXY = 0;

                    // SUM( Xaver - X )^2
                    double sX2 = 0;
                    // SUM( Yaver - Y )^2
                    double sY2 = 0;

                    for(int i = 0; i < testSet.numberOfElements(); i++){
                        double x = testSet.labels().element(i)[0];

                        covXY += (Ycal[i] - mvY)*(x - mvX);
                        sX2 += (x - mvX)*(x - mvX);
                        sY2 += (Ycal[i] - mvY)*(Ycal[i] - mvY);
                    }
                    corr = covXY / sqrt(sX2 * sY2);
                    qsq = 1.0 - deltaSum / sX2;

#ifndef DEBUG
                    if(corr*corr > 0.5){
#endif
                        echoArch(layers);
                        cerr << "MSE: " << mse << std::endl;
                        cerr << "Corr^2: " << corr*corr << std::endl;
                        cerr << "q^2: " << qsq << std::endl;
#ifndef DEBUG
                    }
#endif
                    calibrationSet.shuffle();
                }
            }
        }
    }

    cerr << "Press Enter key ^_^\n";
    std::cin.get();
    return EVALNORM;
    //*/
}
