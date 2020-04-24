#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "network.h"
#include "tools.h" 

int main() {
    struct network* net = InitializeNetwork();
    printf("initialization finished\n");
    for (size_t i = 0; i < 4900; i++)
    {
        printf("feed forward ...\n");
        ForwardPass(net);
        printf("feed forward ok !\n");
        BackwardPropagation(net);
        printf("back propagation ok ! \n");
        printf("Answer of nn : %c\n",RetrieveChar(PosAnswer(net)));
        UpdateBiases(net);
        printf("updated biases !\n");
        UpdateWeights(net);
    }
    free(net);
    return 0;
}

//Intialize a new network
struct network* InitializeNetwork(){

    struct network *network = malloc(sizeof(struct network));
    if (network == NULL){
        errx(1, "Not enough memory!");
    }
    network -> nbInput = 28*28; // Dimension des images données par la segmentation, a voir avec Marius
    network -> nbHidden = 20; // Arbitraire clairement
    network -> nbOutput = 52; // nb de lettres dans l'alphabet *2
    network -> input = calloc(network -> nbInput, sizeof(struct neurons));
    network -> hidden = InitializeLayer(network -> nbHidden);
    network -> output = InitializeLayer(network -> nbOutput);
    if (network-> input == NULL || network->hidden == NULL || network -> output == NULL){
        errx(1, "Not enough memory!");
    }
    network -> ErrorRate = 0.0;
    network -> MaxErrorRate = 0.0;
    network -> eta = 0.5;
    network -> alpha = 0.9;
    return network;

}

//Intialize a new network
struct neurons* InitializeLayer(size_t nb){
    struct neurons *layer = calloc(nb, sizeof(struct neurons));
    if (layer == NULL){
        errx(1, "Not enough memory!");
    }
    for (size_t i = 0 ; i < nb ; i++)
    {
        layer[i] = InitializeNeurons();
    }
    return layer;
}

//Intialize a new network
struct neurons InitializeNeurons(){
    struct neurons *neuron = malloc(sizeof(struct neurons));
    if (neuron == NULL){
        errx(1, "Not enough memory!");
    }
    neuron -> activation = random(); //Fonction random()
    neuron -> weight = random();
    neuron -> biais = random();
    neuron -> delta = 0.0;
    neuron -> deltaweight = 0.0;
    return *neuron;
}

void ForwardPass(struct network *net)
{
    for (size_t i = 0; i < net-> nbHidden; i++)
    {
        double activation = net->hidden[i].biais;
        for (size_t j = 0; j < net -> nbInput; j++)
        {
            activation += net->input[i*net->nbHidden+j] *net->hidden[i*net -> nbHidden+j].weight;
        }
        net->hidden[i].activation = sigmoid(activation);
    }
    for (size_t j = 0; j < net->nbOutput; j++)
    {
        double activation = net->output[j].biais;
        for (size_t k = 0; k < net->nbHidden; k++)
        {
            activation += net->hidden[k].activation*net->output[k*net->nbOutput+j].weight;
        }
        net->output[j].activation = sigmoid(activation);
    }
    
}

void BackwardPropagation(struct network *net)
{
  for (size_t o = 0; o < net -> nbOutput; o++)
  {
    net -> output[o].delta = (/*net -> goal[o] -*/ net -> output[o].activation) * dSigmoid(net -> output[o].activation);
  }

  double sum;
  for (size_t h = 0; h < net -> nbHidden; h++)
  {
    sum = 0.0;
    for (size_t o = 0; o < net -> nbOutput; o++)
    {
      sum += net->hidden[h*net->nbHidden+o].weight * net->output[o].delta;
    }
    net -> hidden[h].delta = sum * dSigmoid(net->hidden[h].activation);
  }
}

void UpdateWeights(struct network *net)
{
    for (size_t h = 0; h < net -> nbHidden; h++)
    {
        for(size_t i = 0; i < net -> nbInput; i++)
        {
            double output = net -> input[i];
            double error = net -> hidden[h*net->nbHidden+i].delta;
            double dWeight = net -> hidden[h*net->nbHidden+i].deltaweight;

            net -> hidden[h*net->nbHidden+i].weight += net->eta * net->hidden[h*net->nbHidden+i].delta * net->input[i] + net->alpha * dWeight;
            net -> hidden[h*net->nbHidden+i].deltaweight = net->eta * net->hidden[h*net->nbHidden+i].delta * output;
        }
    }
}

void UpdateBiases(struct network *net)
{
    for (size_t h = 0; h < net -> nbHidden; h++)
    {
        net -> hidden[h].biais += net->eta * net -> hidden[h].delta;
    }

    //Update BiasO
    for (size_t o = 0; o < net -> nbOutput; o++)
    {
        net -> output[o].biais += net->eta * net-> output[o].delta;
    }
}
