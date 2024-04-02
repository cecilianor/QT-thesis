#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QObject>
#include <QTest>

#include "Evaluator.h"

class UnitTesting : public QObject
{
    Q_OBJECT

private:
    const QString expressionTestPath = ":/unitTestResources/expressionTest.json";
    QJsonObject mutableExpressionsObject;
    const QJsonObject expressionsObject() const { return mutableExpressionsObject; }

    void setExpressionsObject(QJsonObject obj)
    {
        mutableExpressionsObject = obj;
    }

    // Checks if expected and gotten double floating point values are within 0.0001 of each other.
    bool validDoubleRange(double double1, double double2)
    {
        auto epsilon = 0.0001;
        if (std::abs(double1 - double2) > epsilon)
            return false;
        if (std::abs(double2 - double1) > epsilon)
            return false;
        return true;
    }

private slots:
    void initTestCase();
    void resolveExpression_with_coalesce_value();
    void resolveExpression_with_get_value();
    void resolveExpression_with_has_value();
    void resolveExpression_with_in_value();
    void resolveExpression_with_equals_value();
    void resolveExpression_with_inequality_value();
    void resolveExpression_with_greater_than_value();
    void resolveExpression_with_all_value();
    void resolveExpression_with_case_value();
    void resolveExpression_with_match_value();
    void resolveExpression_with_interpolate_value();
    void resolveExpression_with_compound_value();
    void cleanupTestCase();
};

QTEST_MAIN(UnitTesting)
#include "unittest_evaluator.moc"


void UnitTesting::initTestCase()
{
    QFile file;

    // Verify that the test file can be opened.
    file.setFileName(expressionTestPath);
    QJsonDocument doc;
    if(!file.open(QIODevice::ReadOnly))
        QFAIL("Could not open file: " + expressionTestPath.toUtf8());

    // Verify that the test file got parsed correctly.
    QJsonParseError parseError;
    doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if(parseError.error != QJsonParseError::NoError)
        QFAIL("Could not parse JSON file into a QJsonDocument: " + expressionTestPath.toUtf8());

    QJsonObject obj = doc.object();
    setExpressionsObject(obj);

    file.close();
}

// Test resolve expression function when the get value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_get_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("get").toObject();
    feature.featureMetaData.insert("class", "grass");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"get\" function returns empty result when a string is expected");


    QVERIFY2(result.typeId() == QMetaType::Type::QString, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"get\" function, expected %1 but got %2")
                       .arg("grass",result.toString());
    QVERIFY2(result.toString() == "grass", errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"get\" function returns a valid variant when the result is expected to be non-valid");
    QVERIFY2(result.isValid() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `has` value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_has_value() {
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("has").toObject();
    feature.featureMetaData.insert("subclass", "farm");
    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"has\" function returns empty result when a bool is expected");

    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"has\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"has\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"has\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `in` value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_in_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("in").toObject();
    feature.featureMetaData.insert("class", "residential");
    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"in\" function returns empty result when a bool is expected");

    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"in\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"in\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"in\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());

}


// Test resolve expression function when the `==` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_equals_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("==").toObject();
    feature.featureMetaData.insert("class", "neighbourhood");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"equal\" function returns empty result when a bool is expected");

    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"equal\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"equal\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"equal\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());

    expression = expressionObject.value("specialCase1").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"equal\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"equal\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("specialCase2").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"equal\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"equal\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `!=` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_inequality_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("!=").toObject();
    feature.featureMetaData.insert("class", "neighbourhood");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"not equal\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"not equal\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"not equal\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"not equal\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `>` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_greater_than_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value(">").toObject();
    feature.featureMetaData.insert("intermittent", 1);

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"greater\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"greater\" function, expected %1 but got %2")
                       .arg(true)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"greater\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"greater\" function, expected %1 but got %2")
                       .arg(false)
                       .arg(result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `all` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_all_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("all").toObject();
    feature.featureMetaData.insert("class", "neighbourhood");
    feature.featureMetaData.insert("intermittent", 1);
    feature.featureMetaData.insert("subclass", "farm");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"all\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"all\" function, expected %1 but got %2")
                       .arg(true, result.toBool());
    QVERIFY2(result.toBool() == true, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"all\" function returns empty result when a bool is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Bool, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"all\" function, expected %1 but got %2")
                       .arg(false,result.toBool());
    QVERIFY2(result.toBool() == false, errorMessage.toUtf8());
}


// Test resolve expression function when the `case` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_case_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;
    bool validDoubleError=false;

    //Parse the json file into a QJsonDocument for further processing.
    QJsonObject expressionObject = expressionsObject().value("case").toObject();
    feature.featureMetaData.insert("class", "neighbourhood");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"case\" function returns empty result when an int is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Double || result.typeId() == QMetaType::Type::LongLong, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"case\" function, expected %1 but got %2")
                       .arg(15)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), 15);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"case\" function returns empty result when an int is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Double || result.typeId() == QMetaType::Type::LongLong, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"case\" function, expected %1 but got %2")
                       .arg(20, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), 20);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());
}

// Test resolve expression function when the `coalesce` expression object value is passed in.
// This function checks both for a valid (positive) and invalid (negative case).
void UnitTesting::resolveExpression_with_coalesce_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    QJsonObject expressionObject = expressionsObject().value("coalesce").toObject();
    feature.featureMetaData.insert("class", "neighbourhood");

    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"coalesce\" function returns empty result when an int is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::QString, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"coalesce\" function, expected %1 but got %2")
                       .arg("neighbourhood", result.toString());
    QVERIFY2(result.toString() == "neighbourhood", errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"coalesce\" function returns a valid result when an empty variant is expected");
    QVERIFY2(!result.isValid(), errorMessage.toUtf8());
}


// Test resolve expression function when the `match` expression object value is passed in.
// This function checks that the function returns expected values.
void UnitTesting::resolveExpression_with_match_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;

    bool validDoubleError = false;
    feature.featureMetaData.clear();
    feature.featureMetaData.insert("class", "neighbourhood");

    QJsonObject expressionObject = expressionsObject().value("match").toObject();
    expression = expressionObject.value("positive").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"match\" function returns empty result when an int is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Double || result.typeId() == QMetaType::Type::LongLong, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"match\" function, expected %1 but got %2")
                       .arg(2)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), 2);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expression = expressionObject.value("negative").toArray();
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("\"match\" function returns empty result when an int is expected");
    QVERIFY2(result.typeId() == QMetaType::Type::Double || result.typeId() == QMetaType::Type::LongLong, errorMessage.toUtf8());
    errorMessage = QString("Wrong result from \"match\" function, expected %1 but got %2")
                       .arg(4)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), 4);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());
}


// Test resolve expression function when the `interpolate` expression object value is passed in.
// This function checks that the function returns expected values.
void UnitTesting::resolveExpression_with_interpolate_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QJsonArray expression;
    QVariant result;
    bool validDoubleError=false; // Checks if double values are in a valid range
    double expectedInterpolationResult;

    expression = expressionsObject().value("interpolate").toArray();
    feature.featureMetaData.insert("class", "neighbourhood");


    expectedInterpolationResult = 11;
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 0, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(expectedInterpolationResult, result.toDouble());
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    result = Evaluator::resolveExpression(expression, &feature, 3, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 3, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 11 + (2.*2/5);
    result = Evaluator::resolveExpression(expression, &feature, 5, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 5, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 11 + 4.*2/5;
    result = Evaluator::resolveExpression(expression, &feature, 7, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 7, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 13 + 2.*3/3;
    result = Evaluator::resolveExpression(expression, &feature, 10, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 10, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 16;
    result = Evaluator::resolveExpression(expression, &feature, 11, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 11, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 16 + (2.*5/5);
    result = Evaluator::resolveExpression(expression, &feature, 13, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 13, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 16 + (4.*5/5);
    result = Evaluator::resolveExpression(expression, &feature, 15, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 15, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult = 21;
    result = Evaluator::resolveExpression(expression, &feature, 18, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 18, expected %1 but got %2")
                       .arg(expectedInterpolationResult, result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());
}

// Test resolve expression function when the `compound` expression object value is passed in.
// This function checks that the function returns expected values.
void UnitTesting::resolveExpression_with_compound_value()
{
    PolygonFeature feature;
    QString errorMessage;
    QVariant result;
    bool validDoubleError=false;
    double expectedInterpolationResult;

    QJsonObject expressionObject = expressionsObject().value("compound").toObject();
    QJsonArray expression = expressionObject.value("expression1").toArray();

    feature.featureMetaData.insert("class", "motorway");

    expectedInterpolationResult = 0.5;
    result = Evaluator::resolveExpression(expression, &feature, 0, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 0, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(expectedInterpolationResult, result.toDouble());
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult =1 + (1*1.5/4);
    result = Evaluator::resolveExpression(expression, &feature, 7, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 7, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    feature.featureMetaData.insert("brunnel", "bridge");
    expectedInterpolationResult =1 + (1*(-1.)/4);
    result = Evaluator::resolveExpression(expression, &feature, 7, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 7, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult =2;
    result = Evaluator::resolveExpression(expression, &feature, 11, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 11, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    feature.featureMetaData.insert("ramp", 1);
    expectedInterpolationResult =0.5;
    result = Evaluator::resolveExpression(expression, &feature, 11, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 11, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    feature.featureMetaData.clear();
    feature.featureMetaData.insert("class", "service");
    expectedInterpolationResult =0.75;
    result = Evaluator::resolveExpression(expression, &feature, 11, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 11, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult =2+(2*14./4);
    result = Evaluator::resolveExpression(expression, &feature, 18, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 18, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());

    expectedInterpolationResult =16;
    result = Evaluator::resolveExpression(expression, &feature, 30, 0);
    errorMessage = QString("Wrong result from \"interpolate\" function for zoom level 30, expected %1 but got %2")
                       .arg(expectedInterpolationResult)
                       .arg(result.toDouble());
    validDoubleError = validDoubleRange(result.toDouble(), expectedInterpolationResult);
    QVERIFY2(validDoubleError, errorMessage.toUtf8());
}


void UnitTesting::cleanupTestCase()
{
    // Do nothing
}
