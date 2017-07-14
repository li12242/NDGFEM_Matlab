classdef tri < ndg_lib.std_cell.tri & gq_lib.std_cell.gauss_quad_cell
    %TRI Summary of this class goes here
    %   Detailed explanation goes here
    
    methods
        function obj = tri(N)
            obj = obj@ndg_lib.std_cell.tri(N);
            obj = obj@gq_lib.std_cell.gauss_quad_cell(N);
        end
    end
    methods(Access=protected)
        function [rq, sq, tq, wq] = gaussquad_vol_coor(obj, N)
            xw = obj.TriGaussPoints(N);
            rq = xw(:, 1)*2 -1;
            sq = xw(:, 2)*2 -1;
            wq = xw(:, 3)*2;
            tq = zeros(size(rq));
        end
        
        function [rbq, sbq, tbq, wbq, Nfq] = gaussquad_surf_coor(obj, N)
            sind = 1;
            Nfq = zeros(obj.Nface, 1);
            for f = 1:obj.Nface
                cell = gq_lib.std_cell.get_std_cell(N, obj.faceType(f) );
                obj.Nfq = obj.Nfq + cell.Nq;
                Nfq(f) = cell.Nq;
                eind = sind + cell.Nq - 1;
                r = obj.r(obj.Fmask(:, f));
                s = obj.s(obj.Fmask(:, f));
                rbq(sind:eind) = cell.project_node2quad(r);
                sbq(sind:eind) = cell.project_node2quad(s);
                wbq(sind:eind) = cell.wq;
                sind = eind + 1;
            end
            rbq = rbq(:);
            sbq = sbq(:);
            wbq = wbq(:);
            tbq = zeros(size(rbq));
        end
    end
    
    methods(Static)
        function xw = TriGaussPoints(n)
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Function TriGaussPoints provides the Gaussian points and weights %
        % for the Gaussian quadrature of order n for the standard triangles. %
        % http://math2.uncc.edu/~shaodeng/TEACHING/math5172/Lectures/Lect_15.PDF %
        % Input: n - the order of the Gaussian quadrature (n<=12) %
        % %
        % Output: xw - a n by 3 matrix: %
        % 1st column gives the x-coordinates of points %
        % 2nd column gives the y-coordinates of points %
        % 3rd column gives the weights %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            xw = zeros(n,3);
            if (n == 1)
            xw=[0.33333333333333 0.33333333333333 1.00000000000000];
            elseif (n == 2)
            xw=[0.16666666666667 0.16666666666667 0.33333333333333
                0.16666666666667 0.66666666666667 0.33333333333333
                0.66666666666667 0.16666666666667 0.33333333333333];
            elseif (n == 3)
            xw=[0.33333333333333 0.33333333333333 -0.56250000000000
                0.20000000000000 0.20000000000000 0.52083333333333
                0.20000000000000 0.60000000000000 0.52083333333333
                0.60000000000000 0.20000000000000 0.52083333333333];
            elseif (n == 4)
            xw=[0.44594849091597 0.44594849091597 0.22338158967801
                0.44594849091597 0.10810301816807 0.22338158967801
                0.10810301816807 0.44594849091597 0.22338158967801
                0.09157621350977 0.09157621350977 0.10995174365532
                0.09157621350977 0.81684757298046 0.10995174365532
                0.81684757298046 0.09157621350977 0.10995174365532];
            elseif (n == 5)
            xw=[0.33333333333333 0.33333333333333 0.22500000000000
                0.47014206410511 0.47014206410511 0.13239415278851
                0.47014206410511 0.05971587178977 0.13239415278851
                0.05971587178977 0.47014206410511 0.13239415278851
                0.10128650732346 0.10128650732346 0.12593918054483
                0.10128650732346 0.79742698535309 0.12593918054483
                0.79742698535309 0.10128650732346 0.12593918054483];
            elseif (n == 6)
            xw=[0.24928674517091 0.24928674517091 0.11678627572638
                0.24928674517091 0.50142650965818 0.11678627572638
                0.50142650965818 0.24928674517091 0.11678627572638
                0.06308901449150 0.06308901449150 0.05084490637021
                0.06308901449150 0.87382197101700 0.05084490637021
                0.87382197101700 0.06308901449150 0.05084490637021
                0.31035245103378 0.63650249912140 0.08285107561837
                0.63650249912140 0.05314504984482 0.08285107561837
                0.05314504984482 0.31035245103378 0.08285107561837
                0.63650249912140 0.31035245103378 0.08285107561837
                0.31035245103378 0.05314504984482 0.08285107561837
                0.05314504984482 0.63650249912140 0.08285107561837];
            elseif (n == 7)
            xw=[0.33333333333333 0.33333333333333 -0.14957004446768
                0.26034596607904 0.26034596607904 0.17561525743321
                0.26034596607904 0.47930806784192 0.17561525743321
                0.47930806784192 0.26034596607904 0.17561525743321
                0.06513010290222 0.06513010290222 0.05334723560884
                0.06513010290222 0.86973979419557 0.05334723560884
                0.86973979419557 0.06513010290222 0.05334723560884
                0.31286549600487 0.63844418856981 0.07711376089026
                0.63844418856981 0.04869031542532 0.07711376089026
                0.04869031542532 0.31286549600487 0.07711376089026
                0.63844418856981 0.31286549600487 0.07711376089026
                0.31286549600487 0.04869031542532 0.07711376089026
                0.04869031542532 0.63844418856981 0.07711376089026];
            elseif (n == 8)
            xw=[0.33333333333333 0.33333333333333 0.14431560767779
                0.45929258829272 0.45929258829272 0.09509163426728
                0.45929258829272 0.08141482341455 0.09509163426728
                0.08141482341455 0.45929258829272 0.09509163426728
                0.17056930775176 0.17056930775176 0.10321737053472
                0.17056930775176 0.65886138449648 0.10321737053472
                0.65886138449648 0.17056930775176 0.10321737053472
                0.05054722831703 0.05054722831703 0.03245849762320
                0.05054722831703 0.89890554336594 0.03245849762320
                0.89890554336594 0.05054722831703 0.03245849762320
                0.26311282963464 0.72849239295540 0.02723031417443
                0.72849239295540 0.00839477740996 0.02723031417443
                0.00839477740996 0.26311282963464 0.02723031417443
                0.72849239295540 0.26311282963464 0.02723031417443
                0.26311282963464 0.00839477740996 0.02723031417443
                0.00839477740996 0.72849239295540 0.02723031417443];
            else
                error('Bad input n');
            end
            return;
        end
    end
    
end

