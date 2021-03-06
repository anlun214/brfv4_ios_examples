#ifndef __brf__cpp__BRFCppExample_hpp
#define __brf__cpp__BRFCppExample_hpp

namespace brf {

class BRFCppExample: public BRFBasicCppExample {

public: std::vector< std::shared_ptr<brf::Point> > _pointsToAdd;
public: int _numTrackedPoints;

public: BRFCppExample() : BRFBasicCppExample(),
	_pointsToAdd(0),
	_numTrackedPoints(0)
{
}

public: void initCurrentExample(brf::BRFManager& brfManager, brf::Rectangle& resolution) {

	brf::trace("BRFv4 - basic - point tracking." + brf::to_string("\n")+
		"Click eg. on your face to add a bunch of points to track.");

	// BRFMode.POINT_TRACKING skips the face detection/tracking entirely.
	// This examples shows that both can be done simultaneously by setting
	// the mode to BRFMode.FACE_TRACKING.

	brfManager.setMode(brf::BRFMode::FACE_TRACKING);

	// Default settings: a patch size of 21 (needs to be odd), 4 pyramid levels,
	// 50 iterations and a small error of 0.0006

	brfManager.setOpticalFlowParams(21, 4, 50, 0.0006);

	// true means:  BRF will remove points if they are not valid anymore.
	// false means: developers handle point removal on their own.

	brfManager.setOpticalFlowCheckPointsValidBeforeTracking(true);

    cv::setMouseCallback("main", onClicked, this);
}

public: void updateCurrentExample(brf::BRFManager& brfManager, brf::DrawingUtils& draw) {

	// We add the pointsToAdd right before an update.
	// If you do that onclick, the tracking might not
	// handle the new points correctly.

	if(_pointsToAdd.size() > 0) {
		brfManager.addOpticalFlowPoints(_pointsToAdd);
		_pointsToAdd.clear();
	}

	brfManager.update();

	draw.clear();

	// Face detection results: a rough rectangle used to start the face tracking.

	draw.drawRects(brfManager.getAllDetectedFaces(),    false, 1.0, 0x00a1ff, 0.5);
	draw.drawRects(brfManager.getMergedDetectedFaces(), false, 2.0, 0xffd200, 1.0);

	// Get all faces. The default setup only tracks one face.

	std::vector< std::shared_ptr<brf::BRFFace> >& faces = brfManager.getFaces();

	for(size_t i = 0; i < faces.size(); i++) {

		brf::BRFFace& face = *faces[i];

		if(	face.state == brf::BRFState::FACE_TRACKING_START ||
				face.state == brf::BRFState::FACE_TRACKING) {

			// Face tracking results: 68 facial feature points.

			draw.drawTriangles(	face.vertices, face.triangles, false, 1.0, 0x00a0ff, 0.4);
			draw.drawVertices(	face.vertices, 2.0, false, 0x00a0ff, 0.4);
		}
	}

	std::vector< std::shared_ptr<brf::Point> >& points = brfManager.getOpticalFlowPoints();
	std::vector< bool >& states = brfManager.getOpticalFlowPointStates();

	// Draw points by state: green valid, red invalid

	for(size_t i = 0, l = points.size(); i < l; i++) {
		if(states[i]) {
			draw.drawPoint(points[i], 2, false, 0x00ff00, 1.0);
		} else {
			draw.drawPoint(points[i], 2, false, 0xff0000, 1.0);
		}
	}

	// ... or just draw all points that got tracked.
	//draw.drawPoints(points, 2, false, 0x00ff00, 1.0);

	if(points.size() != _numTrackedPoints) {
		_numTrackedPoints = (int)points.size();
		brf::trace("Tracking " + brf::to_string(_numTrackedPoints) + " points.");
	}
};

public: static void onClicked(int event, int x, int y, int flags, void* obj) {

	BRFCppExample* _this = (BRFCppExample*)obj;

	if(_this != nullptr && event == cv::EVENT_LBUTTONDOWN) {

		// Add 1 point:

		// pointsToAdd.push(new brfv4.Point(x, y));

		//Add 100 points

		double w = 60.0;
		double step = 6.0;
		double xStart = x - w * 0.5;
		double xEnd = x + w * 0.5;
		double yStart = y - w * 0.5;
		double yEnd = y + w * 0.5;
		double dy = yStart;
		double dx = xStart;

		for(; dy < yEnd; dy += step) {
			for(dx = xStart; dx < xEnd; dx += step) {
				_this->_pointsToAdd.push_back(std::shared_ptr<brf::Point>(new brf::Point(dx, dy)));
			}
		}
	}
}

};

}
#endif // __brf__cpp__BRFCppExample_hpp
