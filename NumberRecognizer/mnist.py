import numpy
import scipy.special
import matplotlib.pyplot as plt

class NN:
    def __init__(self,inputnodes, hiddennodes, outputnodes, learningRate):
        self.inputnodes = inputnodes
        self.hiddennodes = hiddennodes
        self.outputnodes = outputnodes
        self.lr = learningRate
        self.wih = numpy.random.normal(0.0, pow(self.hiddennodes, - 0.5), (self.hiddennodes, self.inputnodes))
        self.who = numpy.random.normal(0.0, pow(self.outputnodes, - 0.5), (self.outputnodes, self.hiddennodes))
        self.activationFunc = lambda x: scipy.special.expit(x)

    def train(self, inputList, targetList):
        inputs = numpy.array(inputList, ndmin=2).T
        targets = numpy.array(targetList, ndmin=2).T

        hiddenOutputs = self.activationFunc(numpy.dot(self.wih, inputs))
        hypothesis = self.activationFunc(numpy.dot(self.who, hiddenOutputs))

        cost = targets - hypothesis
        self.who += self.lr * numpy.dot((cost * hypothesis * (1.0 - hypothesis)), numpy.transpose(hiddenOutputs))

        hiddenErrors = numpy.dot(self.who.T, cost)
        self.wih += self.lr * numpy.dot((hiddenErrors * hiddenOutputs * (1.0 - hiddenOutputs)), numpy.transpose(inputs))

    def query(self, inputList):
        inputs = numpy.array(inputList, ndmin=2).T
        hiddenOutputs = self.activationFunc(numpy.dot(self.wih, inputs))
        return self.activationFunc(numpy.dot(self.who, hiddenOutputs))

trainingDataFileHandler = open("mnist_train_600.csv", "r")
trainingDataList = trainingDataFileHandler.readlines()
trainingDataFileHandler.close()

testDataFileHandler = open("mnist_test_100.csv", "r")
testDataList = testDataFileHandler.readlines()
testDataFileHandler.close()

"""
내가 만든 데이터 셋 불러오기 ! ////////////////////////////////////////////////////
"""
testMyDataFileHandler = open("MyDataSet.csv", "r")
testMyDataList = testMyDataFileHandler.readlines()
testMyDataFileHandler.close()

inputnodes = 784
hiddennodes = 100
outputnodes = 10
learningRate = 0.1

net = NN(inputnodes, hiddennodes, outputnodes, learningRate)

step = [];
stepPerf = [];
epochs = 10

"""
학습시킵니다////////////////////////////////////////////////////////////////////////
"""

for e in range(epochs):
    print('Epoch: ', e + 1)
    for record in trainingDataList:
        all_values = record.split(',')
        image_array = ((numpy.asfarray(all_values[1:]))/255.0 * 0.99) + 0.01

        targets = numpy.zeros(outputnodes) + 0.01
        targets[int(all_values[0])] = 0.99
        net.train(image_array, targets)

resultCard = []
cnt = 0

"""
아래 코드는 원래 테스트 데이터 셋을 불러와 쿼리하는 코드
"""
"""
for record in testDataList:
    cnt = cnt + 1
    all_values = record.split(',')
    target = int(all_values[0])
    
    inputs = (numpy.asfarray(all_values[1:])/255.0 * 0.99) + 0.01
    
    #query the network!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    outputs = net.query(inputs)

    if(numpy.argmax(outputs) == target):
        resultCard.append(1)
    else:
        resultCard.append(0)
    perf = numpy.asarray(resultCard).mean() * 100.0
    step.append(cnt)
    stepPerf.append(perf)
"""

"""
내가 만든 테스트 데이터 셋을 불러와 쿼리하는 코드 ////////////////////////////////////////
"""

cnt = 0

for record in testMyDataList:
    cnt = cnt + 1
    all_values = record.split(',')
    target = int(all_values[0])

    inputs = (numpy.asfarray(all_values[1:])/255.0 * 0.99) + 0.01

    # query the network!!!!!!!!!!!!!!!!!!!!
    outputs = net.query(inputs)

    if(numpy.argmax(outputs) == target):
        resultCard.append(1)
    else:
        resultCard.append(0)
    perf = numpy.asarray(resultCard).mean() * 100.0
    step.append(cnt)
    stepPerf.append(perf)


print("performance = ", perf)
print(step)
print(stepPerf)
plt.plot(step, stepPerf)
plt.show()