def hgcallayout(i, p, *rows): i["HGCAL/Layouts/" + p] = DQMItem(layout=rows)

########### define varialbles for frequently used strings #############
hgcallink = "   >>> <a href=https://hgcaldocs.web.cern.ch/RawDataHandling/dqm_sysval/>Description</a>"
quality = "summary of module status"
summary = "wafer map for hgcal"
digis = "digis information"

tb_modules = [
    'ML_F3WC_IH0197', 'ML_F3WC_IH0196', 'ML_F3WC_IH0198', 'ML_F3WC_IH0190',
    'ML_F3WD_IH0241', 'ML_F3WD_IH0266', 'ML_F3WC_IH0192', 'ML_F3WC_IH0191',
    'ML_F3WC_IH0194', 'ML_F3WC_IH0182', 'ML_F3WC_IH0180', 'TL_L44S1'
]

# Layout configurations for DQM GUI with placeholders: {layer}, {module}
# Format: (gui_display_path, dqm_histogram_path)
# GUI path determines folder hierarchy under the "Layouts" button on the GUI
# (e.g., "TOT/Plot Name" appears under TOT folder)
layout_configs = [
    #----------------------------------------------------------------------
    # Wafer maps under Layouts
    #----------------------------------------------------------------------
    ("Layer {layer}: Average ADC",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/hex_avgadc_layer_{layer}"),

    ("Noise - Layer {layer}: ADC Standard Deviation",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/hex_stdadc_layer_{layer}"),

    #----------------------------------------------------------------------
    # TrigPhase layouts
    #----------------------------------------------------------------------
    ("TrigPhase/Trigger Phase - ADC @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/seedadcvstrigtime"),

    ("TrigPhase/Trigger Phase - ToA @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/seedtoavstrigtime"),

    #----------------------------------------------------------------------
    # ADC layouts
    #----------------------------------------------------------------------
    ("ADC/Average ADC @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/avgadc"),

    #----------------------------------------------------------------------
    # TOT layouts
    #----------------------------------------------------------------------
    ("TOT/Average TOT @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/hex_avgtot_layer_{layer}"),

    ("TOT/TOT @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/tot"),

    #----------------------------------------------------------------------
    # TOA layouts
    #----------------------------------------------------------------------
    ("TOA/Average TOA @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/hex_avgtoa_layer_{layer}"),

    ("TOA/TOA @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/toa"),

    ("TOA/Occupancy @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/Cassette_1/({uv_coor}) {module}/hex_toaoccupancy_module_{idx}"),

    #----------------------------------------------------------------------
    # RecHits layouts
    #----------------------------------------------------------------------
    ("RecHits/Layer {layer}: RecHit Energy vs. TrigPhase",
     "HGCAL/EndCap_Minus/Layer_{layer}/rechitenergyvstrigtime"),

    ("RecHits/ Layer {layer}: RecHit Time vs. TrigPhase",
     "HGCAL/EndCap_Minus/Layer_{layer}/rechittimevstrigtime"),

    ("RecHits/Time vs. Energy @ Layer {layer}",
     "HGCAL/EndCap_Minus/Layer_{layer}/rechittimevsenergy"),
]

################### Links to TOP Summary Histograms #################################

#------------------------------------------------------------------------------------------------------------------------
# Example syntax to add one plot under ADC layout folder:
#
# hgcallayout(dqmitems, "ADC/Layer 1: Average ADC",
#           [{ 'path': "HGCAL/EndCap_Minus/Layer_1/Cassette_1/hex_avgadc_layer_1", 'description': quality + hgcallink }])
#------------------------------------------------------------------------------------------------------------------------

uv_coordinates = ["u8-v5"]*11 + ["u0-v0"]

dqmIdx_modules = [0, 1, 2, 3, 10, 6, 4, 5, 9, 7, 8, 11]

for i, layer in enumerate([1,2,3,4,5,6,7,8,9,10,11,44]):

    module = tb_modules[i]
    uv_coor = uv_coordinates[i]
    idx = dqmIdx_modules[i]

    for title_template, path_template in layout_configs:
        title = title_template.format(layer=layer, module=module, uv_coor=uv_coor, idx=idx)
        path = path_template.format(layer=layer, module=module, uv_coor=uv_coor, idx=idx)

        if (layer==44 and 'hex' in path) and not ('toaoccupancy' in path):
            path = path.replace('Cassette_1/',f'Cassette_1/({uv_coor}) {module}/')
            path = path.replace(f'_layer_{layer}','_module_11')

        hgcallayout(dqmitems, title, [{'path': path, 'description': quality + hgcallink}])

# FED counter
hgcallayout(dqmitems, "FED/Counters - FED1601",
      [{ 'path': "HGCAL/FED/FED_1601/summaryPerModule_FED1601", 'description': quality + hgcallink }])
hgcallayout(dqmitems, "FED/Econ-D Quality - FED1601",
      [{ 'path': "HGCAL/FED/FED_1601/econdQualityFED_1601", 'description': quality + hgcallink }])
