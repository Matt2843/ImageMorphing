#include "resultspreview.h"

ResultsPreview::ResultsPreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Results Preview", ScrollableQGroupBox::Vertical)
{

}

/**
 * @brief ResultsPreview::addMorphResult
 * @param result
 */
void ResultsPreview::addMorphResult(MorphResult result)
{

    ImageContainer *target_copy = new ImageContainer(this);
    target_copy->update(result.getImageContainer(MorphResult::TARGET));
    m_container.insert(target_copy);
    updatePreview();
}
