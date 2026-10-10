// SPDX-License-Identifier: GPL-3.0-only
#include <gmock/gmock.h>

#include "project/internal/opensaveprojectscenario.h"
#include "global/tests/mocks/interactivemock.h"
#include "mocks/projectconfigurationmock.h"

using namespace muse;
using namespace mu::project;
using namespace testing;

class ComposerLocalSaveTest : public Test
{
protected:
    void SetUp() override
    {
        m_context = std::make_shared<modularity::Context>(311);
        m_interactive = std::make_shared<StrictMock<InteractiveMock>>();
        m_configuration = std::make_shared<NiceMock<ProjectConfigurationMock>>();
        m_previousConfiguration = modularity::globalIoc()->resolve<IProjectConfiguration>("composer-local-save-test");
        modularity::globalIoc()->unregister<IProjectConfiguration>("composer-local-save-test");
        modularity::globalIoc()->registerExport<IProjectConfiguration>("composer-local-save-test", m_configuration);
        modularity::ioc(m_context)->registerExport<IInteractive>("composer-local-save-test", m_interactive);
        m_scenario = std::make_unique<OpenSaveProjectScenario>(m_context);
    }

    void TearDown() override
    {
        m_scenario.reset();
        modularity::ioc(m_context)->unregister<IInteractive>("composer-local-save-test");
        modularity::globalIoc()->unregister<IProjectConfiguration>("composer-local-save-test");
        if (m_previousConfiguration) {
            modularity::globalIoc()->registerExport<IProjectConfiguration>("composer-local-save-test", m_previousConfiguration);
        }
    }

    modularity::ContextPtr m_context;
    std::shared_ptr<StrictMock<InteractiveMock>> m_interactive;
    std::shared_ptr<NiceMock<ProjectConfigurationMock>> m_configuration;
    std::shared_ptr<IProjectConfiguration> m_previousConfiguration;
    std::unique_ptr<OpenSaveProjectScenario> m_scenario;
};

TEST_F(ComposerLocalSaveTest, RememberedCloudPreferenceStillUsesTheDiskPicker)
{
    ON_CALL(*m_configuration, lastUsedSaveLocationType()).WillByDefault(Return(SaveLocationType::Cloud));
    ON_CALL(*m_configuration, shouldAskSaveLocationType()).WillByDefault(Return(true));
    const io::path_t destination("/scores/My score.mscz");
    EXPECT_CALL(*m_interactive, selectSavingFileSync(_, _, _, _)).WillOnce(Return(destination));
    const auto result = m_scenario->askSaveLocation(nullptr, SaveMode::SaveAs);
    ASSERT_TRUE(result.ret);
    EXPECT_TRUE(result.val.isLocal());
    EXPECT_EQ(result.val.localPath(), destination);
}

TEST_F(ComposerLocalSaveTest, CancellingDiskPickerCancelsSave)
{
    EXPECT_CALL(*m_interactive, selectSavingFileSync(_, _, _, _)).WillOnce(Return(io::path_t()));
    const auto result = m_scenario->askSaveLocation(nullptr, SaveMode::SaveAs);
    EXPECT_EQ(result.ret.code(), int(Ret::Code::Cancel));
}

TEST_F(ComposerLocalSaveTest, ExplicitCloudDestinationIsRejected)
{
    const auto result = m_scenario->askSaveLocation(nullptr, SaveMode::SaveAs, SaveLocationType::Cloud);
    EXPECT_EQ(result.ret.code(), int(Ret::Code::NotSupported));
}
