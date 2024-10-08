#ifndef EDepSim_RooTrackerKinematicsGenerator_hh
#define EDepSim_RooTrackerKinematicsGenerator_hh
////////////////////////////////////////////////////////////
//

#include <vector>

#include "kinem/EDepSimVKinematicsGenerator.hh"

class TFile;
class TTree;
class TBits;
class TObjString;
class G4Event;

namespace EDepSim {class KinemPassThrough;}

/// This is an interface to read rooTracker formatted kinematic info. 
namespace EDepSim {class RooTrackerKinematicsGenerator;}
class EDepSim::RooTrackerKinematicsGenerator : public EDepSim::VKinematicsGenerator {
friend class EDepSim::KinemPassThrough;
public:
    ///  Construct a new generator.  The name is the name of the generator
    ///  (e.g. NEUT, GENIE, &c).  The fileName is the name of the root file
    ///  containing the tree of kinematic information.  The treeName is the
    ///  path of the rooTracker tree in the input file.  The order is a string
    ///  specifying the order that events in the input files should be used.
    ///  The current legal values are "consecutive", "stride", or
    ///  "random". The firstEvent is the first event to use in the file.
    ///
    ///  While NEUT and GENIE produce files with single interactions in each
    ///  event, the individual events can be built into spill using another
    ///  program (a so called overlay program).  The divisions between the
    ///  spills can be specified by adding in a fake event that has a single
    ///  particle with a HepStatus value of -1.  The fake event will be
    ///  dropped from generation, so it should not contain a real particle.
    RooTrackerKinematicsGenerator(const G4String& name, 
                                      const G4String& fileName,
                                      const G4String& treeName,
                                      const G4String& order,
                                      int firstEvent);
    virtual ~RooTrackerKinematicsGenerator();

    /// Add a primary vertex to the event.  
    virtual GeneratorStatus GeneratePrimaryVertex(
        G4Event* evt, const G4LorentzVector& position);

    /// Get the name of the open kinematics file.
    virtual G4String GetInputName();

private:
    /// The static part of the file name field.
    std::string fFilename;

    /// The RooTracker file to read.
    TFile* fInput;

    /// The event tree that contains the output events.
    TTree *fTree;

    /// The next entry to read from the file.  The entry to be used is
    /// fEntryVector[fNextEntry].
    unsigned int fNextEntry;

    /// A pre-filled vector of entry numbers to be used from input tree.  This
    /// is used to allow the order of the input interactions to be randomized.
    /// Beam flux simulators typically over-sample pion decays in the beam
    /// pipe so that they can generate larger samples.  This has the
    /// unfortunate effect that consecutive neutrino interactions will have
    /// correlated energies.
    std::vector<int> fEntryVector;

    //////////////////////////////////////////////////////////////
    // Declare the information to get from the ntuple.  This does not get all
    // of the information, only the stuff that is actually used.
    //////////////////////////////////////////////////////////////

    /// The generator-specific event flags.
    TBits*       fEvtFlags;

    /// The generator-specific string with the 'event code'
    TObjString*  fEvtCode;

    /// The sequence number of the event (the event number).
    int         fEvtNum;

    /// The cross section for the event (1E-38 cm2)
    double      fEvtXSec;

    /// The differential cross section for the event kinematics 
    /// (1E-38 cm2/{K^n})
    double      fEvtDXSec;

    /// The weight for the event
    double      fEvtWght;

    /// The probability for the event (given the cross section, path lengths,
    /// etc.).
    double      fEvtProb;

    /// The event vertex position in detector coord syst (in meters and
    /// seconds).
    double      fEvtVtx[4];

    /// The number of particles in the particle arrays to track
    int         fStdHepN;

    /// The maximum number of particles that can be in the particle arrays.
    static const int kNPmax = 4000;

    /// The PDG codes for the particles to track.  This may include generator
    /// specific codes for pseudo particles.
    int         fStdHepPdg[kNPmax]; //[fStdHepN]

    /// The a generator specific status for each particle.  Particles with a
    /// status equal to 1 will be tracked.
    ///
    /// The HEPEVT status values are as follows:
    /// - 0 : null entry.
    /// - 1 : an existing entry, which has not decayed or fragmented. This is
    ///    the main class of entries, which represents the `final state' given
    ///    by the generator.
    /// - 2 : an entry which has decayed or fragmented and is therefore not
    ///    appearing in the final state, but is retained for event history
    ///    information.
    /// - 3 : a documentation line, defined separately from the event
    ///    history. This could include the two incoming reacting particles,
    ///    etc.
    /// - 4 to 10 :
    ///    undefined, but reserved for future standards. 
    /// - 11 to 200 : at the disposal of each model builder for constructs
    ///    specific to his program, but equivalent to a null line in the
    ///    context of any other program.
    /// - 201 and above : at the disposal of users, in particular for event
    /// tracking in the detector.
    ///
    /// The GENIE generator approximately follows the HEPEVT status codes.
    /// As of July 2008, the status values found the GENIE source code are:
    ///   - -1 -- Undefined particle
    ///   -  0 -- An initial state particle.
    ///   -  1 -- A stable final state particle to be tracked.
    ///   -  2 -- An intermediate particle that should not be tracked.
    ///   -  3 -- A particle which decayed and should not be tracked.  If 
    ///            this particle produced daughters to be tracked, they will
    ///            have a state of 1.
    int         fStdHepStatus[kNPmax]; //[fStdHepN]

    /// The position (x, y, z, t) (fm, second) of the particle in the nuclear
    /// frame
    double      fStdHepX4[kNPmax][4]; //[fStdHepN]

    /// The 4-momentum (px, py, pz, E) of the particle in the LAB frame (GeV)
    double      fStdHepP4[kNPmax][4]; //[fStdHepN]

    /// The particle polarization vector.
    double      fStdHepPolz  [kNPmax][3]; //[fStdHepN]

    /// The index of the first daughter of the particle in the arrays.
    int         fStdHepFd[kNPmax]; //[fStdHepN]

    /// The index last daughter of the particle in the arrays.
    int         fStdHepLd[kNPmax]; //[fStdHepN]

    /// The index of the first mother of the particle in there arrays.
    int         fStdHepFm[kNPmax]; //[fStdHepN]

    /// The index of the last mother of the particle in the arrays.
    int         fStdHepLm[kNPmax]; //[fStdHepN]

    //////////////////////////////
    /// The following variables are copied more or less directly from the
    /// input flux generator.
    //////////////////////////////

    /// The PDG code of the particle which created the parent neutrino.
    int         fNuParentPdg;

    /// The interaction mode at the vertex which created the parent neutrino.
    /// This is normally the decay mode of the parent particle.
    int         fNuParentDecMode;

    /// The 4 momentum of the particle at the vertex which created the parent
    /// neutrino.  This is normally the momentum of the parent particle at the
    /// decay point.
    double      fNuParentDecP4[4];

    /// The position of the vertex at which the neutrino was created.  This is
    /// passed directly from the beam (or other flux) generator, and is in the
    /// native units of the original generator.
    double      fNuParentDecX4[4];

    /// The momentum of the parent particle at it's production point.  This is
    /// in the native energy units of the flux generator.
    double      fNuParentProP4[4];
    
    /// The position of the parent particle at it's production point.  This
    /// uses the target as the origin and is in the native units of the flux
    /// generator.
    double      fNuParentProX4[4];

    /// The vertex ID of the parent particle vertex.
    int         fNuParentProNVtx;
};
#endif
