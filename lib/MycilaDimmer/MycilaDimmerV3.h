// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include "MycilaDimmer.h"
#include <Arduino.h>

#define MYCILA_DIMMER_NOT_LUT

// level                    : |    0|    1|    2|    3|    4|    5|    6|    7|    8|    9|   10|   11|   12|   13|   14|   15|   16|   17|   18|   19|   20|   21|   22|   23|   24|   25|   26|   27|   28|   29|   30|   31|   32|   33|   34|   35|   36|   37|   38|   39|   40|   41|   42|   43|   44|   45|   46|   47|   48|   49|   50|   51|   52|   53|   54|   55|   56|   57|   58|   59|   60|   61|   62|   63|   64|   65|   66|   67|   68|   69|   70|   71|   72|   73|   74|   75|   76|   77|   78|   79|   80|   81|   82|   83|   84|   85|   86|   87|   88|   89|   90|   91|   92|   93|   94|   95|   96|   97|   98|   99|  100|  101|  102|  103|  104|  105|  106|  107|  108|  109|  110|  111|  112|  113|  114|  115|  116|  117|  118|  119|  120|  121|  122|  123|  124|  125|  126|  127|  128|  129|  130|  131|  132|  133|  134|  135|  136|  137|  138|  139|  140|  141|  142|  143|  144|  145|  146|  147|  148|  149|  150|  151|  152|  153|  154|  155|  156|  157|  158|  159|  160|  161|  162|  163|  164|  165|  166|  167|  168|  169|  170|  171|  172|  173|  174|  175|  176|  177|  178|  179|  180|  181|  182|  183|  184|  185|  186|  187|  188|  189|  190|  191|  192|  193|  194|  195|  196|  197|  198|  199|  200|  201|  202|  203|  204|  205|  206|  207|  208|  209|  210|  211|  212|  213|  214|  215|  216|  217|  218|  219|  220|  221|  222|  223|  224|  225|  226|  227|  228|  229|  230|  231|  232|  233|  234|  235|  236|  237|  238|  239|  240|  241|  242|  243|  244|  245|  246|  247|  248|  249|  250|  251|  252|  253|  254|  255|
// dimmable_light_linearized: | 9984| 9837| 9695| 9557| 9425| 9297| 9173| 9054| 8939| 8827| 8720| 8617| 8517| 8421| 8329| 8240| 8154| 8071| 7992| 7915| 7841| 7770| 7702| 7636| 7573| 7512| 7454| 7397| 7343| 7291| 7241| 7193| 7147| 7102| 7060| 7018| 6979| 6941| 6904| 6869| 6834| 6802| 6770| 6739| 6710| 6681| 6654| 6627| 6601| 6576| 6552| 6529| 6506| 6484| 6462| 6441| 6420| 6400| 6380| 6361| 6342| 6323| 6305| 6287| 6269| 6251| 6234| 6216| 6199| 6182| 6165| 6148| 6131| 6114| 6097| 6080| 6063| 6046| 6029| 6012| 5995| 5977| 5960| 5942| 5925| 5907| 5889| 5870| 5852| 5834| 5815| 5796| 5777| 5758| 5738| 5719| 5699| 5679| 5659| 5639| 5618| 5597| 5576| 5555| 5534| 5513| 5491| 5469| 5447| 5425| 5403| 5381| 5358| 5336| 5313| 5290| 5267| 5244| 5221| 5198| 5175| 5151| 5128| 5104| 5081| 5057| 5034| 5010| 4987| 4963| 4940| 4916| 4893| 4869| 4846| 4822| 4799| 4776| 4753| 4730| 4707| 4684| 4661| 4638| 4616| 4594| 4571| 4549| 4527| 4506| 4484| 4463| 4441| 4420| 4399| 4379| 4358| 4338| 4318| 4298| 4278| 4258| 4239| 4219| 4200| 4181| 4163| 4144| 4126| 4108| 4090| 4072| 4054| 4036| 4019| 4001| 3984| 3967| 3950| 3933| 3916| 3899| 3882| 3865| 3848| 3831| 3814| 3796| 3779| 3762| 3744| 3726| 3709| 3690| 3672| 3653| 3634| 3615| 3595| 3575| 3554| 3533| 3511| 3489| 3466| 3442| 3418| 3393| 3367| 3341| 3313| 3284| 3255| 3224| 3193| 3160| 3126| 3090| 3053| 3015| 2975| 2934| 2891| 2847| 2800| 2752| 2702| 2650| 2596| 2540| 2481| 2420| 2357| 2291| 2223| 2152| 2078| 2001| 1921| 1839| 1753| 1664| 1571| 1475| 1375| 1272| 1165| 1053|  938|  819|  695|  567|  434|  297|  154|    7|

// dimmable_light_linearized:
// 25% power: 63 (25%)
// 50% power: 127 (50%)
// 75% power: 191 (75%)

uint16_t Mycila::Dimmer::_lookupFiringDelay(uint8_t level, float frequency) {
  if (level == 0)
    return 500000 / frequency; // semi-period in microseconds

  level = map(level, 0, MYCILA_DIMMER_MAX_LEVEL, 0, 255);

  if (level == MYCILA_DIMMER_MAX_LEVEL)
    return 0;

  // // https://github.com/fabianoriccardi/dimmable-light/blob/main/src/dimmable_light_linearized.h
  if (frequency == 60)
    return -1.2528e-7 * pow(level, 5) + 7.9866e-5 * pow(level, 4) - 1.9126e-2 * pow(level, 3) + 2.1225 * powf(level, 2) - 124.71 * level + 8320.1;
  else
    return -1.5034e-7 * pow(level, 5) + 9.5843e-5 * pow(level, 4) - 2.2953e-2 * pow(level, 3) + 2.5471 * pow(level, 2) - 149.65 * level + 9984.6;
}

float Mycila::Dimmer::_lookupVrmsFactor(uint8_t level, float frequency) {
  if (level == 0)
    return 0;

  if (level == MYCILA_DIMMER_MAX_LEVEL)
    return 1;

  // if (level > MYCILA_DIMMER_MAX_LEVEL - 3)
  //   return 1;
  // level += 2;

  const uint16_t delay = _lookupFiringDelay(level, frequency);
  const float angle = Mycila::Dimmer::_delayToPhaseAngle(delay, frequency);
  return Mycila::Dimmer::_vrmsFactor(angle);
}
