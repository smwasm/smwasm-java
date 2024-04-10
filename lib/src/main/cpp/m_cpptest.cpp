#include "c_base.h"

int ct_sn = 0;

smjson::Json TestBeat(smjson::Json sj)
{
    ct_sn++;

    std::string text = sj["text"].string_value();

    smjson::Json ret = smjson::Json::object{
        {"beat", ct_sn},
        {"text", text},
        {"for", "test cpp sm"}};
    return ret;
}

void int_cpptest()
{
    sm_register("native.heart.beat", (SmFunc *)TestBeat);
}
