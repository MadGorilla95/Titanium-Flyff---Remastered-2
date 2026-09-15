QT += widgets
requires(qtConfig(treeview))

SOURCES       = \
    ChangeCW.cpp \
    CollisionAuto.cpp \
    DAEExporter.cpp \
    DialogEditEffects.cpp \
    DialogListModel.cpp \
    Exporter.cpp \
    GeneratedFiles/qrc_MainFrame.cpp \
    ImportAnim.cpp \
    ImportGeometry.cpp \
    ImportMaterial.cpp \
    ImportSkel.cpp \
    ImportUtils.cpp \
    Importer.cpp \
    MainFrame.cpp \
    ModelViewer.cpp \
    OBJExporter.cpp \
    SkinAuto.cpp \
    main.cpp \
    stdafx.cpp

# install
target.path = $$F:\try\ATools-0.3\WorldEditor
INSTALLS += target

RESOURCES += \
    MainFrame.qrc \
    MainFrame.qrc

FORMS += \
    CDialogListModel.ui \
    DialogEditEffects.ui \
    MainFrame.ui \
    SkinAuto.ui

HEADERS += \
    ContinentDef.h \
    DAEExporter.h \
    DialogEditEffects.h \
    DialogListModel.h \
    Exporter.h \
    GeneratedFiles/ui_CDialogListModel.h \
    GeneratedFiles/ui_DialogEditEffects.h \
    GeneratedFiles/ui_MainFrame.h \
    GeneratedFiles/ui_SkinAuto.h \
    ImportUtils.h \
    Importer.h \
    MainFrame.cpp.autosave \
    MainFrame.h \
    ModelEditor.rc \
    ModelViewer.h \
    OBJExporter.h \
    ResData.h \
    SkinAuto.h \
    WndStyle.h \
    afxres.h \
    define.h \
    defineAttribute.h \
    defineItem.h \
    defineItemkind.h \
    defineJob.h \
    defineNeuz.h \
    defineObj.h \
    defineSkill.h \
    defineSound.h \
    defineText.h \
    defineWorld.h \
    definequest.h \
    lang.h \
    resource.h \
    stdafx.h

DISTFILES += \
    D3DCompiler_43.dll \
    D3dx9d_43.dll \
    DSETUP.dll \
    FloatToString.dll \
    ModelEditor-d.ilk \
    ModelEditor-d.pdb \
    ModelEditor.exe \
    ModelEditor.vcxproj \
    ModelEditor.vcxproj.filters \
    ModelEditor.vcxproj.user \
    Modeleditor.pro.user \
    NeuzD.dll \
    Qt5Core.dll \
    Qt5Cored.dll \
    Qt5Gui.dll \
    Qt5Guid.dll \
    Qt5Multimedia.dll \
    Qt5MultimediaQuick.dll \
    Qt5MultimediaWidgets.dll \
    Qt5Multimediad.dll \
    Qt5Network.dll \
    Qt5NetworkAuth.dll \
    Qt5NetworkAuthd.dll \
    Qt5Networkd.dll \
    Qt5Svg.dll \
    Qt5Svgd.dll \
    Qt5Widgets.dll \
    Qt5Widgetsd.dll \
    Qt5Xml.dll \
    Qt5Xmld.dll \
    Skin.vs \
    Terrain.inc \
    World.inc \
    WorldDialog-x64.dll \
    WorldDialog.dll \
    assimp-vc140-mt.dll \
    assimp-vc141-mt.dll \
    character-etc.inc \
    character-school.inc \
    character.inc \
    d3dx9_43.dll \
    dbghelp.dll \
    dirview.pro.user \
    fmod.dll \
    fmod64.dll \
    icudt53.dll \
    icuin53.dll \
    icuuc53.dll \
    jnHYnd.json \
    language_english.png \
    language_french.png \
    language_german.png \
    mdlDyna.inc \
    mdlObj.inc \
    modeleditor_de.qm \
    modeleditor_de.ts \
    modeleditor_en.qm \
    modeleditor_en.ts \
    mss32.dll \
    msvcp120.dll \
    msvcr120.dll \
    npkcrypt.dll \
    npkpdb.dll \
    propItem.txt \
    propKarma.txt \
    propMotion.txt \
    propMover.txt \
    propQuest-DungeonandPK.inc \
    propQuest-RequestBox.inc \
    propQuest-RequestBox2.inc \
    propQuest-Scenario.inc \
    propQuest.inc \
    propSkill.txt \
    propSkillAdd.csv \
    propTroupeSkill.txt \
    s.cflags \
    s.config \
    s.creator.user \
    s.cxxflags \
    s.files \
    s.includes \
    textClient.inc
