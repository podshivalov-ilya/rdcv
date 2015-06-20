#include "lrmodel.h"

#define DEBUG

LRModel::LRModel()
{
    nValidationSet = 4;
}

bool LRModel::loadData(std::string dataFileName, std::string labelsFileName)
{
    shark::Data<shark::RealVector> data;
    shark::Data<shark::RealVector> labels;

    try{
        shark::importCSV(labels, labelsFileName, ' ', '#');
        shark::importCSV(data, dataFileName, ' ', '#');
    } catch (shark::Exception e){
        cerr << "Exception!\n\t" << e.file() << "\t" << e.line() << "\t" << e.what() << std::endl;
        cerr << "Unable to open file " <<  dataFileName << " and/or " << labelsFileName << ". Check paths!" << std::endl;
        return false;
    }
    dataset = shark::LabeledData<shark::RealVector, shark::RealVector> (data, labels);

    //in_cnt = shark::dataDimension(dataset.inputs());
    return true; }

int LRModel::evaluate()
{
    cerr << "dataset NOE: " << dataset.numberOfElements() << std::endl;
    shark::RegressionDataset validationSet = shark::splitAtElement(dataset, static_cast<size_t>((1.0 - 1.0/nValidationSet) * dataset.numberOfElements()));
    cerr << "dataset NOE: " << dataset.numberOfElements() << std::endl;
    cerr << "vset NOE: " << validationSet.numberOfElements() << std::endl;

    trainer.train(model, dataset);

    cerr << "Intercept: " << model.offset() << std::endl;
    cerr << "Matrix: " << model.matrix() << std::endl;

    shark::Data<shark::RealVector> prediction = model(validationSet.inputs());
    cerr << "MSE: " << loss(validationSet.labels(), prediction) << std::endl;

    cerr << "pred NOE: " << prediction.numberOfElements() << std::endl;
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
    std::vector<double> Ycal;     // Observed array
    // delta = SUM( Y - X )^2
    double deltaSum = 0;     // Sum of squared differ between calculated and observed. There is upper half of q^2
#ifdef DEBUG
    cerr << "Y\tX\tdelta^2\n";
#endif
    for(int i = 0; i < validationSet.numberOfElements() - 1; i++){
        // getting i-ths observed value
        double x = validationSet.labels().element(i)[0];
        // evaluate i-ths set to Y
        deltaSum += (prediction.element(i)[1] - x) * (prediction.element(i)[1] - x);
#ifdef DEBUG
        cerr << prediction.element(i)[1] << '\t' << x << '\t' << (prediction.element(i)[1] - x)*(prediction.element(i)[1] - x) << std::endl;
#endif
        mse += (prediction.element(i)[1] - x)*(prediction.element(i)[1] - x);

        mvX += x;
        mvY += prediction.element(i)[1];
        Ycal.push_back(prediction.element(i)[1]);
    }
    mse = mse / validationSet.numberOfElements();
    mvX /= validationSet.numberOfElements();  // X average divides by N (there is for averaging. Lol)
    mvY /= validationSet.numberOfElements();  // Y average divides by N (there is for averaging. Lol)

    //        SUM( X - Xaver) * (Y - Yaver )
    // corr = sqrt(SUM( Xaver - X )^2 * SUM( Yaver - Y )^2 )
    double corr = 0;

    // covXY = SUM( X - Xaver ) * (Y - Yaver )
    double covXY = 0;

    // SUM( Xaver - X )^2
    double sX2 = 0;
    // SUM( Yaver - Y )^2
    double sY2 = 0;

    for(int i = 0; i < validationSet.numberOfElements() - 1; i++){
        double x = validationSet.labels().element(i)[0];

        covXY += (Ycal[i] - mvY)*(x - mvX);
        sX2 += (x - mvX)*(x - mvX);
        sY2 += (Ycal[i] - mvY)*(Ycal[i] - mvY);
    }
    corr = covXY / sqrt(sX2 * sY2);
    qsq = 1.0 - deltaSum / sX2;

#ifndef DEBUG
    if(corr*corr > 0.9){
#endif
        //echoArch(layers);
        cerr << "MSE: " << mse << std::endl;
        cerr << "Corr^2: " << corr*corr << std::endl;
        cerr << "q^2: " << qsq << std::endl;
#ifndef DEBUG
    }
#endif
    //*/
}
