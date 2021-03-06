// Minimalistic example for the MimoProcessor with JACK.

#include "apf/mimoprocessor.h"
#include "apf/combine_channels.h"  // for apf::CombineChannelsCopy
#include "apf/jack_policy.h"

class MyProcessor : public apf::MimoProcessor<MyProcessor, apf::jack_policy>
{
  public:
    using Input = MimoProcessorBase::DefaultInput;
    class Output;

    MyProcessor();
};

class MyProcessor::Output : public MimoProcessorBase::DefaultOutput
{
  public:
    explicit Output(const Params& p)
      : MimoProcessorBase::DefaultOutput(p)
      , _combiner(this->parent.get_input_list(), *this)
    {}

    APF_PROCESS(Output, MimoProcessorBase::DefaultOutput)
    {
      _combiner.process(my_predicate());
    }

  private:
    struct my_predicate
    {
      // trivial, all inputs are used
      apf::CombineChannelsResult::type select(const Input&)
      {
        return apf::CombineChannelsResult::constant;
      }
    };

    apf::CombineChannelsCopy<rtlist_proxy<Input>, DefaultOutput> _combiner;
};

MyProcessor::MyProcessor()
  : MimoProcessorBase()
{
  this->add<Input>();
  this->add<Output>();
}

int main()
{
  MyProcessor processor;
  processor.activate();
  std::this_thread::sleep_for(std::chrono::seconds(30));
  processor.deactivate();
}
