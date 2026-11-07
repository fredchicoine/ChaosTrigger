#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include <iostream>
// Generates 4 test loops, plays a held note, checks audio + lane switching, snapshots GUI.
int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    ChaosTriggerProcessor p;
    const double sr = 48000; const int bs = 512;
    auto dir = juce::File (argv[1]);
    for (int l = 0; l < 4; ++l)
    {
        juce::AudioBuffer<float> b (2, (int) sr * 2);
        for (int i = 0; i < b.getNumSamples(); ++i)
        {
            float env = std::exp (-(float) (i % 12000) / (1500.0f * (l + 1)));
            float v = env * std::sin (i * 0.02f * (l + 1)) * 0.8f;
            b.setSample (0, i, v); b.setSample (1, i, v);
        }
        auto f = dir.getChildFile ("Amen Break Lane " + juce::String (l + 1) + " 150bpm.wav");
        f.deleteFile();
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::AudioFormatWriter> w (wav.createWriterFor (new juce::FileOutputStream (f), sr, 2, 24, {}, 0));
        w->writeFromAudioSampleBuffer (b, 0, b.getNumSamples());
        w.reset();
        if (! p.loadSample (l, f)) { std::cerr << "load failed\n"; return 1; }
    }
    p.setPlayConfigDetails (0, 2, sr, bs);
    p.prepareToPlay (sr, bs);
    p.apvts.getParameter ("l2_chorus")->setValueNotifyingHost (1.0f);
    p.apvts.getParameter ("m_fmode")->setValueNotifyingHost (p.apvts.getParameter ("m_fmode")->convertTo0to1 (1));
    p.apvts.getParameter ("m_cutoff")->setValueNotifyingHost (0.7f);
    p.apvts.getParameter ("l3_smash")->setValueNotifyingHost (0.8f);
    p.apvts.getParameter ("l1_dist")->setValueNotifyingHost (0.5f);
    p.apvts.getParameter ("l4_haas")->setValueNotifyingHost (0.2f);

    juce::AudioBuffer<float> buf (2, bs);
    juce::AudioBuffer<float> all (2, (int) sr * 3);
    std::set<int> lanesSeen; float peak = 0; bool nan = false;
    for (int blk = 0; blk * bs < all.getNumSamples() - bs; ++blk)
    {
        juce::MidiBuffer m;
        if (blk == 0) m.addEvent (juce::MidiMessage::noteOn (1, 60, (juce::uint8) 100), 0);
        if (blk == 200) m.addEvent (juce::MidiMessage::noteOff (1, 60), 0);
        p.processBlock (buf, m);
        lanesSeen.insert (p.activeLane.load());
        for (int c = 0; c < 2; ++c)
        {
            all.copyFrom (c, blk * bs, buf, c, 0, bs);
            for (int i = 0; i < bs; ++i) { float v = buf.getSample (c, i); nan |= ! std::isfinite (v); peak = std::max (peak, std::abs (v)); }
        }
    }
    std::cout << "peak=" << peak << " nan=" << nan << " lanes seen:";
    for (int l : lanesSeen) std::cout << " " << l;
    std::cout << "\n";
    {
        auto f = dir.getChildFile ("render.wav"); f.deleteFile();
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::AudioFormatWriter> w (wav.createWriterFor (new juce::FileOutputStream (f), sr, 2, 24, {}, 0));
        w->writeFromAudioSampleBuffer (all, 0, all.getNumSamples());
    }
    std::unique_ptr<juce::AudioProcessorEditor> ed (p.createEditor());
    ed->setSize (1400, 920);
    p.activeLane = 1;
    auto img = ed->createComponentSnapshot (ed->getLocalBounds(), true, 1.0f);
    juce::PNGImageFormat png; juce::FileOutputStream os (dir.getChildFile ("gui.png"));
    os.setPosition (0); os.truncate(); png.writeImageToStream (img, os);
    return (peak > 0.01f && ! nan && lanesSeen.size() >= 4) ? 0 : 2;
}
